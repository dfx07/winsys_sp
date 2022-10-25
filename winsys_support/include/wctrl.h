#pragma once
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

class Button : public Control
{
	enum { WIDTH_DEF = 80 };
	enum { HEIGHT_DEF = 25 };

protected:
	int         m_x;
	int         m_y;
	UINT        m_width;
	UINT        m_height;

	wstring     m_label;
	int         m_bClicked;

	HBITMAP hBmp;
	HBRUSH	m_background;

	void(*m_EventFun)(Window* window, Button* btn) = NULL;
public:
	Button() : Control(CtrlType::BUTTON),
		m_width(WIDTH_DEF), m_height(HEIGHT_DEF),
		m_x(0) , m_y(0)
	{
		hBmp = GetHBITMAPFromImageFile(L"resources\\plus24.bmp");
		m_background = NULL;
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
		m_hwnd = (HWND)CreateWindowEx(NULL, L"BUTTON", m_label.c_str(),     // Button text 
			//WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_NOTIFY,  // Styles 
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | BS_PUSHBUTTON | BS_NOTIFY,
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

		return Control::OnInitControl(IDS);
	}

	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		if (!m_EventFun) return;

		m_EventFun(window, this);
	}

	void SetEvent(void(*mn)(Window*, Button*))
	{
		m_EventFun = mn;
	}

	void OwnerDrawButton()
	{

	}

	HBRUSH CreateGradientBrush(HDC hdc, RECT rect, COLORREF top, COLORREF bottom)
	{
		HBRUSH Brush = NULL;
		HDC hdcmem = CreateCompatibleDC(hdc);
		HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hdcmem, hbitmap);

		int r1 = GetRValue(top), r2 = GetRValue(bottom), g1 = GetGValue(top), g2 = GetGValue(bottom), b1 = GetBValue(top), b2 = GetBValue(bottom);
		for (int i = 0; i < rect.bottom - rect.top; i++)
		{
			RECT temp;
			int r, g, b;
			r = int(r1 + double(i * (r2 - r1) / rect.bottom - rect.top));
			g = int(g1 + double(i * (g2 - g1) / rect.bottom - rect.top));
			b = int(b1 + double(i * (b2 - b1) / rect.bottom - rect.top));
			Brush = CreateSolidBrush(RGB(r, g, b));
			temp.left = 0;
			temp.top = i;
			temp.right = rect.right - rect.left;
			temp.bottom = i +1;
			FillRect(hdcmem, &temp, Brush);
			DeleteObject(Brush);
		}
		HBRUSH pattern = CreatePatternBrush(hbitmap);

		DeleteDC(hdcmem);
		DeleteObject(Brush);
		DeleteObject(hbitmap);

		return pattern;
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

	void DrawBackground(LPDRAWITEMSTRUCT& pdis)
	{
		if (!m_background)
		{
			m_background = CreateGradientBrush(pdis->hDC, pdis->rcItem, RGB(180, 0, 0), RGB(255, 180, 0));
		}

		if (!m_background)
			return;

		//Create pen for button border
		HPEN pen = CreatePen(PS_INSIDEFRAME, 0, RGB(100, 100, 100));

		//HGDIOBJ old_pen = SelectObject(pdis->hDC, pen);
		HGDIOBJ old_brush = SelectObject(pdis->hDC, m_background);

		RoundRect(pdis->hDC, pdis->rcItem.left, pdis->rcItem.top, pdis->rcItem.right, pdis->rcItem.bottom, 10, 10);

		//SelectObject(pdis->hDC, old_pen);
		SelectObject(pdis->hDC, old_brush);

		DeleteObject(pen);
	}

	void Draw(LPDRAWITEMSTRUCT& pdis)
	{
		UINT uMsg = pdis->itemAction;
		
		BITMAP          bitmap01;
		HDC             hdcMem01;
		HGDIOBJ         oldBitmap01;

		//CDC* pDC = CDC::FromHandle(hDC);

		DrawBackground(pdis);

		if (m_bClicked)
		{
			//HBRUSH brush;
			//brush = CreateSolidBrush(RGB(100, 0, 0));
			//Draw_Border_Rectangles(pdis->rcItem, pdis->hDC, brush);
		}
		else
		{
			//hdcMem01 = CreateCompatibleDC(pdis->hDC);
			//oldBitmap01 = SelectObject(hdcMem01, hBmp);

			//GetObject(hBmp, sizeof(bitmap01), &bitmap01);
			//BitBlt(pdis->hDC, 0, 0, bitmap01.bmWidth, bitmap01.bmHeight, hdcMem01, 0, 0, SRCCOPY);

			//SelectObject(hdcMem01, oldBitmap01);
			//DeleteDC(hdcMem01);

			//HBRUSH brush;
			//brush = CreateSolidBrush(RGB(100, 0, 0));

			//Draw_Border_Rectangles(pdis->rcItem, pdis->hDC, brush);
		}

		// reset state click
		if (pdis->itemState & ODS_SELECTED)
		{
			m_bClicked = TRUE;
		}
		else
		{
			m_bClicked = FALSE;
		}
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
	int r;
	int g;
	int b;
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