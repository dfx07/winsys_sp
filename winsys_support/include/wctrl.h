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
#include "xeasing.h"

class MenuContext;
class MenuBar;
class Window;

using namespace std;

#pragma comment( lib, "MSIMG32.LIB")

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
	HDC  hDC = NULL;
	RECT rect;
};

struct _Color4
{
	float	 r;
	float	 g;
	float	 b;
	float	 a;

	Gdiplus::ARGB wrefcol;

	void set(float r = 255, float g = 255, float b = 255, float a = 255)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
		wrefcol = Gdiplus::Color::MakeARGB(a, r, g, b);
	}

	_Color4(float r = 255, float g = 255, float b = 255, float a =255)
	{
		this->set(r, g, b, a);
	}
};


class MemDC
{
private:
	HBITMAP		oldBmp;
	HBITMAP		newBmp;

	HDC			m_oldhDC;
public:
	HDC			m_hDC;
	RECT		m_rect;
private:
	void CreateBufferDC(const HDC& hdc,const RECT& rect)
	{
		m_oldhDC = hdc;
		m_rect   = rect;
		m_hDC = CreateCompatibleDC(hdc);
		newBmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		// we need to save original bitmap, and select it back when we are done,
		// in order to avoid GDI leaks!
		oldBmp = (HBITMAP)SelectObject(m_hDC, newBmp);
	}

	void DeleteBufferDC()
	{
		SelectObject(m_hDC, oldBmp); // select back original bitmap
		DeleteObject(newBmp); // delete bitmap since it is no longer required
		DeleteDC(m_hDC);   // delete memory DC since it is no longer required
	}

public:
	void Init(const HDC& hdc, const RECT& rect)
	{
		this->CreateBufferDC(hdc, rect);
	}

public:
	MemDC(const win_draw_info& dinfo)
	{
		this->Init(dinfo.hDC, dinfo.rect);
	}

	MemDC():m_hDC(NULL), 
		m_rect()
	{
		
	}

	MemDC(HDC& _hdc, RECT& _rect)
	{
		this->Init(_hdc, _rect);
	}

	void DrawRoundRect(const RECT& rect, const HPEN pen, const HBRUSH brush)
	{
		HGDIOBJ old_pen = NULL;
		HGDIOBJ old_brush = NULL;

		if (pen)
			old_pen = SelectObject(m_hDC, pen);

		old_brush = SelectObject(m_hDC, brush);

		RoundRect(m_hDC, rect.left, rect.top, rect.right, rect.bottom, 0, 0);

		if (pen)
			SelectObject(m_hDC, old_pen);

		SelectObject(m_hDC, old_brush);
	}

	~MemDC()
	{
		this->Flush();
	}

	void Flush()
	{
		BitBlt(m_oldhDC, 0, 0, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, m_hDC, 0, 0, SRCCOPY);

		// all done, now we need to cleanup
		this->DeleteBufferDC();
	}
};

namespace Gdiplus
{
	enum ImageAlignment
	{
		// Left edge for left-to-right text,
		// right for right-to-left text,
		// and top for vertical
		ImageAlignmentNear   = 0,
		ImageAlignmentCenter = 1,
		ImageAlignmentFar    = 2
	};

	class ImageFormat
	{
	public:
		Gdiplus::ImageAlignment m_veralign;
		Gdiplus::ImageAlignment m_horalign;

		Gdiplus::Size			m_offset;
	public:
		void SetVerticalAlignment(Gdiplus::ImageAlignment align)
		{
			m_veralign = align;
		}

		void SetHorizontalAlignment(Gdiplus::ImageAlignment align)
		{
			m_horalign = align;
		}

		void SetOffset(int x, int y)
		{
			m_offset.Width = x;
			m_offset.Height = y;
		}
	};
};


class GDIplusRender
{
private:
	HBITMAP		oldBmp;
	HBITMAP		newBmp;

private:
	HDC			m_oldhDC;
private:
	HDC			m_hDC;
	RECT		m_rect;

	int					m_gdiplustatus;
	ULONG_PTR           m_gdiplusToken;
public:
	Gdiplus::Graphics*	m_render;
	Gdiplus::Rect		m_rect_render;

	Gdiplus::Font*		m_font_render;

private:
	bool CreateGDIplus()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		bool ret = Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL) 
			== Gdiplus::Status::Ok ? true : false;
		return ret;
	}

	void ShutdownGDIplus()
	{
		Gdiplus::GdiplusShutdown(m_gdiplusToken);
	}

	void CreateBufferRender(const HDC& hdc, const RECT& rect)
	{
		m_oldhDC = hdc;
		m_rect   = rect;
		m_hDC = CreateCompatibleDC(hdc);
		newBmp = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		oldBmp = (HBITMAP)SelectObject(m_hDC, newBmp);

		// gdiplus render main
		m_render = Gdiplus::Graphics::FromHDC(m_hDC);
		m_render->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		m_rect_render = GDIplusRender::ConvertToGdiplusRect(rect, -1, -1);
	}

	void DeleteBufferRender()
	{
		delete m_render; m_render = NULL;
		SelectObject(m_hDC, oldBmp); // select back original bitmap
		DeleteObject(newBmp); // delete bitmap since it is no longer required
		DeleteDC(m_hDC);   // delete memory DC since it is no longer required
	}

public:
	GDIplusRender(): m_render(NULL),
		m_hDC(NULL),
		m_font_render(NULL),
		m_gdiplustatus(false)
	{
	}

	~GDIplusRender()
	{
		delete m_font_render;
		this->ShutdownGDIplus();
		this->Flush();
	}

	void Init(const HDC& hdc, const RECT& rect)
	{
		if (m_gdiplustatus == false)
		{
			m_gdiplustatus = this->CreateGDIplus();
		}

		if(m_gdiplustatus)
			this->CreateBufferRender(hdc, rect);
	}

	void LoadFont(const wchar_t* family= L"Arial")
	{
		Gdiplus::FontFamily   fontFamily(family);

		delete m_font_render;
		m_font_render = new Gdiplus::Font(&fontFamily, 12, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
	}

	void Flush()
	{

		// TODO: http://www.winprog.org/tutorial/transparency.html
		BitBlt(m_oldhDC, 0, 0, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, m_hDC, 0, 0, SRCCOPY);

		// all done, now we need to cleanup
		this->DeleteBufferRender();
	}

// Function support draw gdiplus
private:
	static Gdiplus::Rect ConvertToGdiplusRect(const RECT& rect, const int offset_x = 0, const int offset_y = 0)
	{
		return Gdiplus::Rect(rect.left, rect.top, rect.right + offset_x, rect.bottom + offset_y);
	}

	static Gdiplus::RectF Rect2RectF(const Gdiplus::Rect* rect)
	{
		return Gdiplus::RectF(rect->X, rect->Y, rect->Width, rect->Height);
	}

	static void funcDrawRoundRectangle(Gdiplus::Graphics* g, const Gdiplus::Pen* p, const Gdiplus::Rect& rect, const int radius)
	{
		Gdiplus::GraphicsPath path;

		path.AddLine(rect.X + radius, rect.Y, rect.X + rect.Width - (radius * 2), rect.Y);
		path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y, radius * 2, radius * 2, 270, 90);
		path.AddLine(rect.X + rect.Width, rect.Y + radius, rect.X + rect.Width, rect.Y + rect.Height - (radius * 2));
		path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height - (radius * 2), radius * 2,
			radius * 2, 0, 90);
		path.AddLine(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height, rect.X + radius, rect.Y + rect.Height);
		path.AddArc(rect.X, rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 90, 90);
		path.AddLine(rect.X, rect.Y + rect.Height - (radius * 2), rect.X, rect.Y + radius);
		path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
		path.CloseFigure();

		g->DrawPath(p, &path);
	}

	void funcFillRoundRectangle(Gdiplus::Graphics* g, const Gdiplus::Brush* brush, const Gdiplus::Rect& rect,const int radius)
	{
		Gdiplus::GraphicsPath path;

		path.AddLine(rect.X + radius, rect.Y, rect.X + rect.Width - (radius * 2), rect.Y);
		path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y, radius * 2, radius * 2, 270, 90);
		path.AddLine(rect.X + rect.Width, rect.Y + radius, rect.X + rect.Width, rect.Y + rect.Height - (radius * 2));
		path.AddArc(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 0, 90);
		path.AddLine(rect.X + rect.Width - (radius * 2), rect.Y + rect.Height, rect.X + radius, rect.Y + rect.Height);
		path.AddArc(rect.X, rect.Y + rect.Height - (radius * 2), radius * 2, radius * 2, 90, 90);
		path.AddLine(rect.X, rect.Y + rect.Height - (radius * 2), rect.X, rect.Y + radius);
		path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
		path.CloseFigure();

		g->FillPath(brush, &path);
	}

	void funDrawImage(Gdiplus::Graphics* g,
						Gdiplus::Image* img,
						const Gdiplus::ImageFormat* imageformat)
	{
		using namespace Gdiplus;

		Gdiplus::RectF rectbound = Rect2RectF(&m_rect_render);
		Gdiplus::RectF rect = rectbound;

		unsigned int image_width = img->GetWidth();
		unsigned int image_height = img->GetHeight();

		if (imageformat)
		{
			// alignment x
			if (imageformat->m_veralign == Gdiplus::ImageAlignment::ImageAlignmentCenter)
			{
				rect.X = rectbound.X + (rectbound.Width - image_width) / 2;
			}
			else if (imageformat->m_veralign == Gdiplus::ImageAlignment::ImageAlignmentFar)
			{
				rect.X = rectbound.X + rectbound.Width - image_width;
			}
			else
			{
				rect.X = rectbound.X;
			}

			// Y alignment
			if (imageformat->m_horalign == Gdiplus::ImageAlignment::ImageAlignmentCenter)
			{
				rect.Y = rectbound.Y + (rectbound.Height - image_height) / 2;
			}
			else if (imageformat->m_horalign == Gdiplus::ImageAlignment::ImageAlignmentFar)
			{
				rect.Y = rectbound.Y + rectbound.Height - image_height;
			}
			else
			{
				rect.Y = rectbound.Y;
			}

			int rect_width  = image_width < rectbound.Width ? image_width : rectbound.Width;
			int rect_height = image_height < rectbound.Height ? image_height : rectbound.Height;

			rect.Width  = rect_width;
			rect.Height = rect_height;

			rect.X += imageformat->m_offset.Width;
			rect.Y += imageformat->m_offset.Height;
		}

		g->DrawImage(img, rect);
	}

	void funDrawText(Gdiplus::Graphics* g, 
					const Gdiplus::RectF* rect,
					const wchar_t* text,
					const Gdiplus::Font* font,
					const Gdiplus::Brush* brush,
					const Gdiplus::StringFormat* stringFormat)
	{
		g->DrawString(text, -1, font, *rect, stringFormat, brush);
	}

// Draw function
public:
	void DrawRectangle(const Gdiplus::Pen* pen, const Gdiplus::Brush* brush, int radius)
	{
		if (!m_render) return;

		if(brush)
			GDIplusRender::funcFillRoundRectangle(this->m_render, brush, this->m_rect_render, radius);
		if(pen)
			GDIplusRender::funcDrawRoundRectangle(this->m_render, pen, this->m_rect_render, radius);
	}

	void DrawTextFullRect(const wchar_t* text, const Gdiplus::Brush* brush, const Gdiplus::StringFormat* stringFormat = NULL)
	{
		if (!m_render || !brush || !m_font_render) return;

		Gdiplus::RectF rectf = Rect2RectF(&this->m_rect_render);

		GDIplusRender::funDrawText(this->m_render, &rectf, text, m_font_render, brush, stringFormat);
	}

	void DrawImageFullRect(Gdiplus::Image* img, const Gdiplus::ImageFormat* imageFormat = NULL)
	{
		if (!m_render) return;

		GDIplusRender::funDrawImage(this->m_render, img, imageFormat);
	}
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

	Gdiplus::Brush* m_background_normal;
	Gdiplus::Brush* m_backgroundclick;
	Gdiplus::Brush* m_backgroundhover;

	_Color4		  m_normal_color;
	_Color4		  m_hover_color;
	_Color4		  m_hot_color;

	Gdiplus::Bitmap* m_image;

	EasingEngine  m_easing;

	GDIplusRender m_render;

	int			m_border_width;
	int			m_color_click;


	void(*m_EventFun)(Window* window, Button* btn) = NULL;
	void(*m_EventFunEnter)(Window* window, Button* btn) = NULL;
	void(*m_EventFunLeave)(Window* window, Button* btn) = NULL;


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
	}

	~Button()
	{
		delete m_background_normal;
		delete m_backgroundhover;
		delete m_backgroundclick;

		delete m_image;
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
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW  /*| BS_NOTIFY*/,
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

		case WM_ERASEBKGND:
			return TRUE;
			break;
		case WM_CTLCOLORBTN: {
			//SetBkMode((HDC)wParam, TRANSPARENT);
			break;
		}
		}

		return CallWindowProc(getproc(), hwndBtn, uMsg, wParam, lParam);
	}

private:
	const float m_effect_time_update = 5;

	void BeginX1ThemeEffect()
	{
		SetTimer(m_hwnd, IDC_EFFECT_X1, m_effect_time_update, (TIMERPROC)NULL);

		m_easing.Setup(1);
		m_easing.AddExec(EaseType::Expo, EaseMode::In, m_hover_color.r, m_normal_color.r);
		m_easing.AddExec(EaseType::Expo, EaseMode::In, m_hover_color.g, m_normal_color.g);
		m_easing.AddExec(EaseType::Expo, EaseMode::In, m_hover_color.b, m_normal_color.b);

		m_easing.Start();

		delete m_background_normal;
		m_background_normal = new Gdiplus::SolidBrush(m_hover_color.wrefcol);
	}

	bool UpdateX1ThemeEffect()
	{
		m_easing.Update(m_effect_time_update);

		float r = m_easing.Exec(0);
		float g = m_easing.Exec(1);
		float b = m_easing.Exec(2);

		delete m_background_normal;
		m_background_normal = new Gdiplus::SolidBrush(Gdiplus::Color(r, g, b));

		return m_easing.IsActive();
	}

	void EndX1ThemeEffect()
	{
		KillTimer(m_hwnd, IDC_EFFECT_X1);

		delete m_background_normal;
		m_background_normal = new Gdiplus::SolidBrush(m_normal_color.wrefcol);
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

		if ( m_EventFunEnter && state == BtnState::Hover)
		{
			m_EventFunEnter(NULL, this);
		}
		else if (m_EventFunLeave && state == BtnState::Normal)
		{
			m_EventFunLeave(NULL, this);
		}
		m_eState = state;
	}

	void SetEventEnterCallBack(void(*fun)(Window* window, Button* btn))
	{
		this->m_EventFunEnter = fun;
	}
	void SetEventLeaveCallBack(void(*fun)(Window* window, Button* btn))
	{
		this->m_EventFunLeave = fun;
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
			m_normal_color = std::move(_Color4(59, 91, 179));
			m_background_normal = new Gdiplus::SolidBrush(Gdiplus::Color(m_normal_color.wrefcol));
		}
		if (!m_backgroundclick)
		{
			m_hot_color = std::move(_Color4(201, 224, 247));
			m_backgroundclick = new Gdiplus::SolidBrush(Gdiplus::Color(m_hot_color.wrefcol));
		}
		if (!m_backgroundhover)
		{
			m_hover_color = std::move(_Color4(229, 241, 255));
			m_backgroundhover = new Gdiplus::SolidBrush(Gdiplus::Color(m_hover_color.wrefcol));
		}

		if (!m_image)
		{
			std::wstring pFilePath = L"resources/plus48.png";
			m_image = Gdiplus::Bitmap::FromFile(pFilePath.c_str(), false);
		}
	}

	void Draw(LPDRAWITEMSTRUCT& pdis)
	{
		//TODO : draw use swap buffer image (hdc) -> not draw each element (OK)
		m_render.Init(pdis->hDC, pdis->rcItem);
		m_render.LoadFont(L"Segoe UI");
		
		this->CreateColorButton();

		// draw color
		const int radius = 3;
		if (m_eState == BtnState::Click)
		{
			Gdiplus::Pen pen(Gdiplus::Color(255, 98, 162, 228), 2);
			m_render.DrawRectangle(&pen, m_backgroundclick, radius);
		}
		else if (m_eState == BtnState::Hover)
		{
			Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255, 255), 2);
			m_render.DrawRectangle(&pen, m_backgroundhover, radius);
		}
		else
		{
			Gdiplus::Pen pen(Gdiplus::Color(255, 255, 255, 255), 2);
			m_render.DrawRectangle(&pen, m_background_normal, radius);
		}

		// draw image
		if (m_image)
		{
			Gdiplus::ImageFormat imgformat;
			imgformat.SetVerticalAlignment(Gdiplus::ImageAlignmentNear);
			imgformat.SetHorizontalAlignment(Gdiplus::ImageAlignmentCenter);
			m_render.DrawImageFullRect(m_image, &imgformat);
		}

		// draw text
		Gdiplus::SolidBrush* textcolor = NULL;
		
		if (m_eState == BtnState::Hover)
		{
			textcolor = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 0));
		}
		else
		{
			textcolor = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255));
		}

		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);
		format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
		m_render.DrawTextFullRect(this->m_label.c_str(), textcolor, &format);

		delete textcolor;

		m_render.Flush();
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

	void SetColor(float r, float g, float b)
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

		return Control::OnInitControl(IDS);
	}
};