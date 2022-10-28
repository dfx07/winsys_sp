﻿#pragma once
////////////////////////////////////////////////////////////////////////////////////
// File: WinControl.cpp   -- Create and handle event control the window             
// Copyright (C) 2020-2022 Thuong.NV   Created : 22/04/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#include <commctrl.h>
#include <WinUser.h>
#include <windows.h>
#include <vector>
#include <gdiplus.h>
#include "xeasing.h"

class MenuContext;
class MenuBar;
class Window;

using namespace std;

enum CtrlType
{
	BUTTON,
	MENUCONTEXT,
	MENUBAR,
	COMBOBOX,
	LABEL,
};


struct win_draw_info
{
	RECT rect;
	HDC  hDC = NULL;
};

//===================================================================================
//⮟⮟ Lớp control : Lớp cơ sở triển khai các control của Window                    
//===================================================================================
class Control
{
protected:
	UINT         m_ID;
	HWND         m_hwnd;
	HWND         m_hwndPar;
	CtrlType     m_type;

	bool         m_bEnable;
	bool         m_bVisble;

public:
	Control(CtrlType type) :
		m_hwnd(NULL), m_ID(0)
	{
		m_type = type;
	}

public:
	virtual int OnInitControl(UINT& IDS) // 0 false | 1 :true
	{
		if (!m_hwnd || m_ID ==0)
			return 0;
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		return 1;
	}

	virtual CtrlType GetType() = 0;
	virtual int	     IsCreated() { return m_hwnd ? TRUE : FALSE; }

	virtual void     OnDestroy() {};
	virtual void     Draw(LPDRAWITEMSTRUCT& ) {};
	virtual void     Event(Window* window, WORD _id, WORD _event) {};
	virtual bool     ContainID(INT ID) { return false; };
	virtual void	 UpdateFontFromParant()
	{
		if (!m_hwndPar)
			return;

		//HDC hdc = GetDC(m_hwndPar);

		//HFONT hlf = (HFONT)GetCurrentObject(hdc, OBJ_FONT);

		//SendMessage(m_hwnd, WM_SETFONT, (WPARAM)hlf, TRUE);
		//ReleaseDC(m_hwndPar, hdc);
		//HDC hdc = GetDC(m_hwndPar);
		//HFONT font = (HFONT)SendMessage(m_hwndPar, WM_GETFONT, (WPARAM)font, FALSE);
		//HFONT hFont = (HFONT)SendMessage(m_hwndPar, WM_GETFONT, NULL, NULL);

		//SendMessage(m_hwnd, WM_SETFONT, (WPARAM)hFont, TRUE);
		//ReleaseDC(m_hwndPar, hdc);
	}
public:
	HWND GetHwnd() { return m_hwnd; }
	INT  GetID() { return m_ID; }
	void SetParent(HWND hwnd) { m_hwndPar = hwnd; }
	void Enable(bool bEnable)
	{
		m_bEnable = bEnable;
		::EnableWindow(m_hwnd, m_bEnable ? TRUE : FALSE);
	}

	void Visible(bool bVisible)
	{
		m_bVisble = bVisible;
		::ShowWindow(m_hwnd, m_bVisble ? TRUE : FALSE);
	}
};


//===================================================================================
//⮟⮟ Lớp MenuContext : Control quản lý Menucontext                                  
//===================================================================================

class MenuItemBase
{
public:
	UINT			  m_ID;
	UINT			  m_type;
	std::wstring      m_label;

public:
	void(*m_EventFun)(Window* window, Control* ctrl) = NULL;

public:
	MenuItemBase()
	{
		m_ID = 0;
		m_label = L"";
	};

	void   SetEvent(void(*mn)(Window*, Control*)) { m_EventFun = mn; }
	void   SetLabel(wstring lab) { m_label = lab; }
	void   SetType(UINT type) { m_type = type; }
	void   SetID(UINT id) { m_ID = id; }

	wstring GetLabel() { return m_label; }
	UINT   GetType()   { return m_type; }
	UINT   GetID()     { return m_ID; }
};

class MenuContext : public Control
{
protected:
	HMENU                    m_hMenu;
	vector<MenuItemBase>     m_items;

public:
	MenuContext() : Control(CtrlType::MENUCONTEXT),
		m_hMenu()
	{

	}

private:
	bool CreateMenuContext(UINT& IDS)
	{
		m_hMenu = CreatePopupMenu();

		for (int i = 0; i < m_items.size(); i++)
		{
			m_items[i].SetID(IDS++);

			if (m_items[i].GetType() == MF_SEPARATOR)
			{
				AppendMenu(m_hMenu, MF_SEPARATOR, m_items[i].GetID(), NULL);
			}
			else
			{
				AppendMenu(m_hMenu, m_items[i].GetType(), m_items[i].GetID(), m_items[i].GetLabel().c_str());
			}
		}

		return true;
	}

public:
	virtual CtrlType GetType() { return m_type; };

	virtual int OnInitControl(UINT& IDS)
	{
		if (!CreateMenuContext(IDS))
		{
			return 0;
		}
		return Control::OnInitControl(IDS);
	}

	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].m_ID == _id && m_items[i].m_EventFun)
			{
				m_items[i].m_EventFun(window, this);
				break;
			}
		}
	}

	bool ContainID(INT ID)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].m_ID == ID) return true;
		}
		return false;
	}

public:
	void AddItem(const MenuItemBase& item)
	{
		m_items.push_back(item);
	}

	void Show(POINT point)
	{
		if (m_hwndPar)
		{
			ClientToScreen(m_hwndPar, &point);
			TrackPopupMenu(m_hMenu, TPM_RIGHTBUTTON, point.x, point.y, 0, m_hwndPar, NULL);
		}
	}

	friend class Window;
};

//===================================================================================
//⮟⮟ Lớp MenuBarContext : Control quản lý MenuBarContext                            
//===================================================================================

class MenuBarItem
{
protected:

	HMENU                  m_hMenu;
	string                 m_text;

	vector<MenuItemBase>   m_items;

public:
	MenuBarItem(string lab = "") : m_text(lab),
		m_hMenu()
	{

	}

private:

	bool CreateMenuBarItem(UINT& IDS)
	{
		m_hMenu = CreateMenu();
		for (int i = 0; i < m_items.size(); i++)
		{
			m_items[i].SetID(IDS++);

			if (m_items[i].GetType() == MF_SEPARATOR)
			{
				AppendMenu(m_hMenu, MF_SEPARATOR, m_items[i].GetID(), NULL);
			}
			else
			{
				AppendMenu(m_hMenu, m_items[i].GetType(), m_items[i].GetID(), m_items[i].GetLabel().c_str());
			}
		}

		return true;
	}

public:

	void AddItem(MenuItemBase item)
	{
		m_items.push_back(item);
	}

	void SetText(string txt)
	{
		m_text = txt;
	}

	bool ContainID(INT ID)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].m_ID == ID) return true;
		}
		return false;
	}

	bool CallEvent(Window* win, Control* ctrl, INT ID)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].m_ID == ID)
			{
				m_items[i].m_EventFun(win, ctrl);
				return true;
			}
		}
		return false;
	}

	friend class MenuBar;
};



class MenuBar : public Control
{
public:
	HMENU               m_hMenuBar;
	vector<MenuBarItem> m_items;

public:
	MenuBar() : Control(CtrlType::MENUBAR)
	{

	}

protected:

	bool CreateMenuBar(UINT& IDS)
	{
		m_hMenuBar = CreateMenu();

		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].CreateMenuBarItem(IDS))
			{
				// TODO : Created menubar ok
				AppendMenuA(m_hMenuBar, MF_POPUP, (UINT_PTR)m_items[i].m_hMenu, m_items[i].m_text.c_str());
				SetMenu(m_hwndPar, m_hMenuBar);
			}
			else
			{
				cout << "[X]  Create Menubar failed ! " << endl;
			}
		}
		return true;
	}

public:
	virtual CtrlType GetType() { return m_type; };

	virtual int OnInitControl(UINT& IDS)
	{
		if (!CreateMenuBar(IDS))
		{
			return 0;
		}
		return Control::OnInitControl(IDS);
	}

public:

	void AddItem(MenuBarItem item)
	{
		m_items.push_back(item);
	}

	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].CallEvent(window, this, _id))
				return;
		}
	}

	bool ContainID(INT ID)
	{
		for (int i = 0; i < m_items.size(); i++)
		{
			if (m_items[i].ContainID(ID))
				return true;
		}
		return false;
	}
};


//===================================================================================
//⮟⮟ Lớp Button : Control quản lý giao diện và sự kiện Button                       
//===================================================================================





struct _Color3
{
	float	 r;
	float	 g;
	float	 b;
	COLORREF wrefcol;

	void set(float r = 255, float g = 255, float b = 255)
	{
		this->r = r;
		this->g = g;
		this->b = b;

		wrefcol = RGB(r, g, b);
	}

	_Color3(float r = 255, float g = 255, float b= 255)
	{
		this->set(r, g, b);
	}
};

class Button : public Control
{
	enum BtnState
	{
		Click,
		Normal,
		Hover,
	};

	enum { IDC_EFFECT_X1 = 12003 };
	enum { WIDTH_DEF = 80 };
	enum { HEIGHT_DEF = 25 };
private:
	bool		m_track_leave;

protected:
	int         m_x;
	int         m_y;
	UINT        m_width;
	UINT        m_height;

	wstring     m_label;
	BtnState    m_eState;
	BtnState	m_eOldState;

	HBITMAP hBmp;
	HBRUSH	m_background_normal;
	HBRUSH	m_backgroundclick;
	HBRUSH	m_backgroundhover;

	_Color3		  m_normal_color;
	_Color3		  m_hover_color;
	_Color3		  m_hot_color;

	//EasingExpo  m_db;

	EasingEngine  m_easing;

	win_draw_info draw_info;

	int			m_border_width;
	int			m_color_click;


	void(*m_EventFun)(Window* window, Button* btn) = NULL;


	static WNDPROC& getproc()
	{
		static WNDPROC prevWndProc;
		return prevWndProc;
	}

public:
	Button() : Control(CtrlType::BUTTON),
		m_width(WIDTH_DEF), m_height(HEIGHT_DEF),
		m_x(0), m_y(0), m_eState(BtnState::Normal)
	{
		hBmp = GetHBITMAPFromImageFile(L"resources\\plus48.png");
		m_background_normal = NULL;
	}

	HBITMAP GetHBITMAPFromImageFile(const WCHAR* pFilePath)
	{
		Gdiplus::GdiplusStartupInput gpStartupInput;
		ULONG_PTR gpToken;
		Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);
		HBITMAP result = NULL;
		Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(pFilePath, false);
		if (bitmap)
		{
			bitmap->GetHBITMAP(Gdiplus::Color::Transparent, &result);
			delete bitmap;
		}
		Gdiplus::GdiplusShutdown(gpToken);
		return result;
	}
public:
	void SetPosition(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
	void SetSize(int width, int height)
	{
		m_width = width;
		m_height = height;
	}
	void SetLabel(wstring lab) { m_label = lab; }

public:
	int OnInitControl(UINT& IDS)
	{
		UINT BackupIDS = IDS;
		m_ID = IDS++;
		m_hwnd = (HWND)CreateWindow(L"BUTTON", m_label.c_str(),     // Button text 
			//WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_NOTIFY,  // Styles 
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_NOTIFY,
			m_x,                                                    // x position 
			m_y,                                                    // y position 
			m_width,                                                // Button width
			m_height,                                               // Button height
			m_hwndPar,                                              // Parent window
			(HMENU)(UINT_PTR)m_ID,                                  // menu.
			(HINSTANCE)GetWindowLongPtr(m_hwndPar, GWLP_HINSTANCE),
			NULL);
		if (!m_hwnd)  // Hoàn trả nếu nó khởi tạo không thành công
		{
			m_ID = 0;
			IDS = BackupIDS;
			return 0;
		}
		//SetWindowSubclass(m_hwnd, &Button::OwnerDrawButton, m_ID, 0);
		getproc() = (WNDPROC)SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)&ButtonProcHandle);

		return Control::OnInitControl(IDS);
	}

	static void TrackMouse(HWND hwnd)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_HOVER | TME_LEAVE; //Type of events to track & trigger.
		tme.dwHoverTime = 1; //How long the mouse has to be in the window to trigger a hover event.
		tme.hwndTrack = hwnd;
		TrackMouseEvent(&tme);
	}

	static LRESULT CALLBACK ButtonProcHandle(HWND hwndBtn, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Button* btn = (Button*)::GetWindowLongPtr(hwndBtn, GWLP_USERDATA);
		if (!btn) return 0;

		switch (uMsg)
		{
		case WM_MOUSEMOVE:
		{
			if (btn->m_eState == BtnState::Click ||
				btn->m_eState == BtnState::Hover)
				break;

			btn->EndX1ThemeEffect();

			btn->SetState(BtnState::Hover);
			if (!btn->m_track_leave)
			{
				TrackMouse(hwndBtn);
				btn->m_track_leave = true;
			}

			InvalidateRect(hwndBtn, NULL, FALSE);
			break;
		}
		case WM_MOUSELEAVE:
		{
			btn->m_track_leave = false;
			btn->SetState(BtnState::Normal, true);
			btn->BeginX1ThemeEffect();
			InvalidateRect(hwndBtn, NULL, FALSE);
			break;
		}
		case WM_TIMER:
		{
			btn->OnTimer(wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			btn->SetState(BtnState::Click);
			break;
		}
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
		{
			btn->m_eState = btn->m_eOldState;
			break;
		}
		case WM_CTLCOLORBTN:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
		}
		}

		return CallWindowProc(getproc(), hwndBtn, uMsg, wParam, lParam);
	}

private:
	const float m_effect_time_update = 40;

	void BeginX1ThemeEffect()
	{
		//std::cout << ">>> Effect" << std::endl;
		SetTimer(m_hwnd, IDC_EFFECT_X1, m_effect_time_update, (TIMERPROC)NULL);

		m_easing.Setup(1);
		m_easing.AddExec(EaseType::Quint, EaseMode::In, m_hover_color.r, m_normal_color.r);
		m_easing.AddExec(EaseType::Quint, EaseMode::In, m_hover_color.g, m_normal_color.g);
		m_easing.AddExec(EaseType::Quint, EaseMode::In, m_hover_color.b, m_normal_color.b);

		m_easing.Start();

		DeleteObject(m_background_normal);
		m_background_normal = CreateSolidBrush(m_hover_color.wrefcol);
	}

	bool UpdateX1ThemeEffect()
	{
		m_easing.Update(m_effect_time_update);

		float r = m_easing.Exec(0);
		float g = m_easing.Exec(1);
		float b = m_easing.Exec(2);

		DeleteObject(m_background_normal);
		m_background_normal = CreateSolidBrush(RGB(r, g, b));

		return m_easing.IsActive();
	}

	void EndX1ThemeEffect()
	{
		//std::cout << "End Effect" << std::endl;

		KillTimer(m_hwnd, IDC_EFFECT_X1);

		DeleteObject(m_background_normal);
		m_background_normal = CreateSolidBrush(m_normal_color.wrefcol);
	}

	virtual void OnTimer(DWORD wParam)
	{
		switch (wParam)
		{
			case IDC_EFFECT_X1:
			{
				if (this->UpdateX1ThemeEffect())
				{
					InvalidateRect(m_hwnd, NULL, FALSE);
					UpdateWindow(m_hwnd);
				}
				else
				{
					EndX1ThemeEffect();
					InvalidateRect(m_hwnd, NULL, FALSE);
				}

				break;
			}
		}
	}

public:
	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		if (!m_EventFun) return;

		m_EventFun(window, this);
	}

	void SetEvent(void(*mn)(Window*, Button*))
	{
		m_EventFun = mn;
	}

	void SetState(BtnState state, bool free_oldstate = false)
	{
		m_eOldState = (free_oldstate) ? BtnState::Normal : m_eState;
		m_eState = state;
	}

	void Draw_Border_Rectangles(RECT rect, HDC hdc, HBRUSH brush, int thinkness =1)
	{
		int width  = abs(rect.right - rect.left);
		int height = abs(rect.bottom - rect.top);

		RECT leftrect, rightrect, bottomrect;
		leftrect.left = 0;
		leftrect.top = rect.bottom - 266;
		leftrect.right = thinkness;
		leftrect.bottom = height;
		//fill left rect of window for border  
		FillRect(hdc, &leftrect, brush);

		rightrect.left = width - thinkness;
		rightrect.top = rect.bottom - 266;
		rightrect.right = width;
		rightrect.bottom = height;
		//fill right rect of window  
		FillRect(hdc, &rightrect, brush);

		bottomrect.left = 0;
		bottomrect.top = height - thinkness;
		bottomrect.right = width;
		bottomrect.bottom = height;
		//fill bottom rect of window  
		FillRect(hdc, &bottomrect, brush);
	}

	void CreateColorButton()
	{
		if (!m_background_normal)
		{
			m_normal_color = std::move(_Color3(225, 225, 225));
			m_background_normal = CreateSolidBrush(m_normal_color.wrefcol);
		}
		if (!m_backgroundclick)
		{
			m_hot_color = std::move(_Color3(201, 224, 247));
			m_backgroundclick = CreateSolidBrush(m_hot_color.wrefcol);
		}
		if (!m_backgroundhover)
		{
			m_hover_color = std::move(_Color3(229, 241, 251));
			m_backgroundhover = CreateSolidBrush(m_hover_color.wrefcol);
		}
	}

	void DrawBackground(HDC& hDC, RECT& rect, HPEN pen, HBRUSH background)
	{
		HGDIOBJ old_pen = NULL;
		HGDIOBJ old_brush = NULL;

		if (pen)
			old_pen = SelectObject(hDC, pen);

		old_brush = SelectObject(hDC, background);

		RoundRect(hDC, rect.left, rect.top, rect.right, rect.bottom, 0, 0);

		if(pen)
			SelectObject(hDC, old_pen);

		SelectObject(hDC, old_brush);
	}

	void DrawButtonClick()
	{
		HPEN pen = CreatePen(PS_INSIDEFRAME, 0, RGB(98, 162, 228));

		DrawBackground(draw_info.hDC, draw_info.rect, pen, m_backgroundclick);

		DeleteObject(pen);
	}

	void DrawButtonNormal()
	{
		HPEN pen = CreatePen(PS_INSIDEFRAME, 0, RGB(180, 180, 180));

		DrawBackground(draw_info.hDC, draw_info.rect, pen, m_background_normal);

		DeleteObject(pen);
	}

	void DrawButtonHover()
	{
		HPEN pen = CreatePen(PS_INSIDEFRAME, 0, RGB(164, 206, 249));

		DrawBackground(draw_info.hDC, draw_info.rect, pen, m_backgroundhover);

		DeleteObject(pen);
	}

	void DrawButtonText()
	{
		if (m_eState == BtnState::Click)
		{
			SetTextColor(draw_info.hDC, RGB(255, 0, 0));
		}

		DrawText(draw_info.hDC, m_label.c_str(), -1, &draw_info.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	void DrawImage()
	{
		HDC     hdcMem01;
		HGDIOBJ oldBitmap01;
		BITMAP  bitmap01;

		BLENDFUNCTION fnc;
		fnc.BlendOp = AC_SRC_OVER;
		fnc.BlendFlags = 0;
		fnc.SourceConstantAlpha = 0xFF;
		fnc.AlphaFormat = AC_SRC_ALPHA;

		hdcMem01  = CreateCompatibleDC(draw_info.hDC);
		oldBitmap01 = SelectObject(hdcMem01, hBmp);

		GetObject(hBmp, sizeof(bitmap01), &bitmap01);
		BitBlt(draw_info.hDC, 0, 0, bitmap01.bmWidth, bitmap01.bmHeight, hdcMem01, 0, 0, SRCCOPY);

		//AlphaBlend(draw_info.hDC, 0, 0, bitmap01.bmWidth, bitmap01.bmHeight, hdcMem01, 0, 0, bitmap01.bmWidth, bitmap01.bmHeight, fnc);

		SelectObject(hdcMem01, oldBitmap01);
	}

	void Draw(LPDRAWITEMSTRUCT& pdis)
	{
		draw_info.rect = pdis->rcItem;
		draw_info.hDC  = pdis->hDC;

		//this->DrawImage();

		this->CreateColorButton();

		if (m_eState == BtnState::Click )
		{
			this->DrawButtonClick();
		}
		else if (m_eState == BtnState::Hover)
		{
			this->DrawButtonHover();
		}
		else
		{
			this->DrawButtonNormal();
		}


		this->DrawButtonText();

		
	}

	virtual CtrlType GetType() { return m_type; };
};


//===================================================================================
//⮟⮟ Lớp Combobox : Control quản lý giao diện và sự kiện Combobox                   
//===================================================================================

struct CBB_ITEM
{
	wstring     text; // dữ liệu text hiển thị trên cbb
	void*       data; // dữ liêu của item tự định nghĩa và kiểm soát
};

class Combobox : public Control
{
	enum { WIDTH_DEF = 100 };
	enum { HEIGHT_DEF = 50 };
private:
	int              m_x;
	int              m_y;
	UINT             m_width;
	UINT             m_height;
	bool             m_editText;

	int              selected;
	vector<CBB_ITEM> items;


	void(*m_EventSelectedChangedFun)(Window*, Combobox*) = NULL;

public:
	Combobox(int _x = 0, int _y = 0, int _width = WIDTH_DEF,
		int _height = HEIGHT_DEF) :Control(CtrlType::COMBOBOX)
	{
		m_x = _x;
		m_y = _y;
		m_width = _width;
		m_height = _height;
		m_editText = false;
		selected = -1;
	}

	~Combobox()
	{
		for (int i = 0; i < items.size(); i++)
		{
			delete items[i].data;
		}
	}
private:

	void UpdateItems()
	{
		if (!m_hwnd) return;

		SendMessage(m_hwnd, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

		for (int i = 0; i < items.size(); i++)
		{
			SendMessage(m_hwnd, CB_ADDSTRING, i, (LPARAM)items[i].text.c_str());
		}

		//SetWindowPos(NULL, NULL, m_x, m_y, m_width, m_height,SWP_NOSIZE|SWP_NOZORDER);

		UpdateSelect();
	}

	void UpdateSelect()
	{
		if (!m_hwnd) return;
		SendMessage(m_hwnd, CB_SETCURSEL, (WPARAM)selected, (LPARAM)0);
		selected = GetSelectIndexItem();
	}

	//===================================================================================
	// Lấy ra chỉ số được select  : nó sẽ được lưu trữ  vào biến seleted                 
	//===================================================================================
	int GetSelectIndexItem()
	{
		if (!m_hwnd)
		{
			selected = -1;
		}
		else
		{
			selected = (int)SendMessage(m_hwnd, CB_GETCURSEL, NULL, NULL);
		}
		return selected;
	}

public:

	void SetPosition(int _x, int _y)
	{
		m_x = _x;
		m_y = _y;
	}

	void SetSize(int width, int height)
	{
		m_width = width;
		m_height = height;
	}

	void SetEventSelectedChange(void(*fun)(Window*, Combobox*))
	{
		m_EventSelectedChangedFun = fun;
	}

	void SetSelect(int index)
	{
		selected = index;
	}

	void TextEdit(bool bEn)
	{
		m_editText = bEn;
	}

	void SetMinItemVisiable(int iMinVisible)
	{
		if (!m_hwnd) return;
		SendMessage((m_hwnd), CB_SETMINVISIBLE, (WPARAM)iMinVisible, 0);
	}

	// Chú ý cần clone và tạo data bằng new 
	void AddItem(wstring text, void* data = NULL)
	{
		CBB_ITEM    item;
		item.text = text;
		item.data = data;

		items.push_back(item);
	}

	//===================================================================================
	// Xóa một item được chỉ định bằng text : tất cả các item có text sẽ bị xóa          
	//===================================================================================
	void RemoveItem(wstring text)
	{
		for (auto it = items.begin(); it != items.end(); /*it++*/)
		{
			if (it->text == text)
			{
				delete it->data;
				it = items.erase(it);
			}
			else ++it;
		}

		UpdateItems();
	}

	//===================================================================================
	// Xóa một item được chỉ định bằng index : tất cả các item có text sẽ bị xóa         
	//===================================================================================
	void RemoveItem(int index)
	{
		if (index < 0 || index >= items.size())
		{
			return;
		}

		delete items[index].data;
		items.erase(items.begin() + index);

		UpdateItems();
	}

	//===================================================================================
	// Xóa toàn bộ item đang co trong combobxo                                           
	//===================================================================================
	void RemoveAllItem()
	{
		for (int i = 0; i < items.size(); i++)
		{
			delete items[i].data;
		}
		items.clear();

		UpdateItems();
	}

	//===================================================================================
	// Xóa toàn bộ item đang co trong combobxo                                           
	//===================================================================================
	void SelectItem(int sel)
	{
		if (sel < 0 || sel >= items.size())
		{
			sel = -1;
		}
		UpdateSelect();
	}

	//===================================================================================
	// Lấy text của item dựa vào index                                                   
	//===================================================================================
	wstring GetItemText(int index)
	{
		if (index < 0 || index >= items.size())
		{
			return L"";
		}

		return items[index].text;
	}

	//===================================================================================
	// Lấy giá trị của item                                                              
	//===================================================================================
	void* GetItemData(int index)
	{
		if (index < 0 || index >= items.size())
		{
			return NULL;
		}

		return items[index].data;
	}

	//===================================================================================
	// Lấy giá trị text của item selected                                                
	//===================================================================================
	wstring GetSelectText()
	{
		GetSelectIndexItem();
		if (selected < 0 || selected >= items.size())
		{
			return L"";
		}

		return items[selected].text;
	}

	//===================================================================================
	// Lấy dữ liệu item đang select                                                      
	//===================================================================================
	void* GetSelectData()
	{
		GetSelectIndexItem();

		if (selected < 0 || selected >= items.size())
		{
			return NULL;
		}

		return items[selected].data;
	}

	//===================================================================================
	// Lấy chỉ số của item selected                                                      
	//===================================================================================
	int GetSelectIndex()
	{
		GetSelectIndexItem();
		return selected;
	}

	virtual CtrlType GetType() { return m_type; }

public:
	virtual int OnInitControl(UINT& IDS)
	{
		m_ID = IDS++;

		int style = WS_CHILD | WS_VISIBLE;

		if (m_editText) style |= CBS_DROPDOWN;
		else            style |= CBS_DROPDOWNLIST;

		m_hwnd = CreateWindow(L"Combobox", NULL, style,      //
			m_x, m_y,										// x, y
			m_width, m_height*(int)items.size(),			// chiều rộng / chiều cao
			m_hwndPar,										// Handle cha
			(HMENU)(UINT_PTR)m_ID, NULL, NULL);

		// Create combobox failed !
		if (!m_hwnd)
		{
			m_ID = 0;
			IDS--;

			return 0;
		}

		UpdateItems();
		return Control::OnInitControl(IDS);
	}

	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		if (_event == CBN_SELCHANGE)
		{
			GetSelectIndexItem();
			if (m_EventSelectedChangedFun)  m_EventSelectedChangedFun(window, this);
		}
	}
};

struct FoxColor {
	float r;
	float g;
	float b;
};


class Label : public Control
{
private:
	std::wstring m_text;

	int			 m_x;
	int			 m_y;

	FoxColor	 m_color;

	int			 m_width = 100;
	int			 m_height =30;

private:
	void UpdateText()
	{
		if (!m_hwnd) return;
		SetWindowText(m_hwnd, m_text.c_str());
	}
	virtual CtrlType GetType() { return m_type; }


	void CalcTextSize(int& width, int& height)
	{
		SIZE size;
		HDC hdc = GetDC(m_hwnd);
		GetTextExtentPoint32(hdc, m_text.c_str(), wcslen(m_text.c_str()), &size);
		
		width = size.cx;
		height = size.cy;

		ReleaseDC(m_hwnd, hdc);
	}
public:
	void UpdateTextColor(HDC hdc)
	{
		if (!hdc) return;

		SetTextColor(hdc, RGB(m_color.r, m_color.g, m_color.b));
	}

public:
	Label(const wchar_t* text = L"")
		: Control(CtrlType::LABEL)
	{
		m_text = text;

		m_color.r = 255;
		m_color.g = 255;
		m_color.b = 255;
	}

	void SetText(const wchar_t* text)
	{
		m_text = text;
		this->UpdateText();
	}

	void SetColor(int r, int g, int b)
	{
		m_color.r = r;
		m_color.g = g;
		m_color.b = b;

		this->UpdateTextColor(NULL);
	}

	void SetPosition(int x, int y)
	{
		m_x = x;
		m_y = y;
	}

public:
	virtual int OnInitControl(UINT& IDS)
	{
		m_ID = IDS++;

		int style = WS_VISIBLE | WS_CHILD ;

		this->CalcTextSize(m_width, m_height);

		m_hwnd = CreateWindow(L"STATIC", m_text.c_str(),
					style,											// style + control name
					m_x, m_y,										// x, y
					m_width, m_height,								// width / height
					m_hwndPar,										// Handle parent
					(HMENU)(UINT_PTR)m_ID,							// ID
					NULL, NULL);

		// Create combobox failed !
		if (!m_hwnd)
		{
			m_ID = 0;
			IDS--;
			return 0;
		}

		//this->SetText(m_text.c_str());

		return Control::OnInitControl(IDS);
	}
};