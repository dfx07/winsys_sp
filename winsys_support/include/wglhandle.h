﻿////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
* @Copyright (C) 2021-2022 thuong.nv <thuong.nv.mta@gmail.com>
*            All rights reserved.
************************************************************************************
* @file     wglhandle.h
* @create   Nov 15, 2022
* @brief    Create and handle event window
* @note     For conditions of distribution and use, see copyright notice in readme.txt
************************************************************************************/
#ifndef WGLHANDLE_H
#define WGLHANDLE_H

#include <assert.h>

#include <stdlib.h>
#include <string.h>
#include <unordered_map>

#include <map>
#include <stack>
#include <mutex>

#include <chrono>
#include <windows.h>
#include <gdiplus.h>

#include "GL/glew.h"
#include "GL/wglew.h"

#include "wglctrl.h"
#include "glrender.h"
#include "xsystype.h"

#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glew32.lib")

#define addAtribute(attribs, name, value)\
{\
    assert((size_t) attribCount < attribsize);\
    attribs[attribCount++] = name;  \
    attribs[attribCount++] = value; \
}

#define addAtributeEnd(attribs)\
{\
    assert((size_t) attribCount < attribsize); \
    attribs[attribCount++] = 0;  \
}

#define GL_WIN_CLASS  L"FOX_WINHANDLE_CLASS"

#define   GL_PRESSED    1
#define   GL_RELEASE    0

___BEGIN_NAMESPACE___

/// ////////////////////////////////////////////////////////////////////////////////
/// GDIplusToken struct - WINDOW
/// Provides a container GDI data
/// ////////////////////////////////////////////////////////////////////////////////
struct GDIplusToken
{
	ULONG_PTR						 m_id;
	Gdiplus::GdiplusStartupInput     m_prop;
};

//Không sử dụng Graphics vì nó bị đỏ khi Bug khó chịu @^@
typedef Gdiplus::Graphics                   WndGDIplus;

enum GLKeyboard
{
    AltLeft         = 0x12,      // (18)
    AltRight        = 0x12,      // (18)
    CapsLock        = 0x14,      // (20)
    ControlLeft     = 0x11,      // (17)
    ControlRight    = 0x11,      // (17)
    OSLeft          = 0x5B,      // (91)
    OSRight         = 0x5C,      // (92)
    ShiftLeft       = 0x10,      // (16)
    ShiftRight      = 0x10,      // (16)
    Delete          = 0x2E,      // (46)
    Escapex         = 0x1B,      // (27)

    Digit1          = 0x31,      // (49)
    Digit2          = 0x32,      // (50)
    Digit3          = 0x33,      // (51)
    Digit4          = 0x34,      // (52)
    Digit5          = 0x35,      // (53)
    Digit6          = 0x36,      // (54)
    Digit7          = 0x37,      // (55)
    Digit8          = 0x38,      // (56)
    Digit9          = 0x39,      // (57)
    Digit0          = 0x30,      // (48)
    KeyA            = 0x41,      // (65)
    KeyB            = 0x42,      // (66)
    KeyC            = 0x43,      // (67)
    KeyD            = 0x44,      // (68)
    KeyE            = 0x45,      // (69)
    KeyF            = 0x46,      // (70)
    KeyG            = 0x47,      // (71)
    KeyH            = 0x48,      // (72)
    KeyI            = 0x49,      // (73)
    KeyJ            = 0x4A,      // (74)
    KeyK            = 0x4B,      // (75)
    KeyL            = 0x4C,      // (76)
    KeyM            = 0x4D,      // (77)
    KeyN            = 0x4E,      // (78)
    KeyO            = 0x4F,      // (79)
    KeyP            = 0x50,      // (80)
    KeyQ            = 0x51,      // (81)
    KeyR            = 0x52,      // (82)
    KeyS            = 0x53,      // (83)
    KeyT            = 0x54,      // (84)
    KeyU            = 0x55,      // (85)
    KeyV            = 0x56,      // (86)
    KeyW            = 0x57,      // (87)
    KeyX            = 0x58,      // (88)
    KeyY            = 0x59,      // (89)
    KeyZ            = 0x5A,      // (90)


    Left            = 0x25,      // (37)   Left arrow.
    Up              = 0x26,      // (38)   Up arrow.
    Right           = 0x27,      // (39)   Right arrow.
    Down            = 0x28,      // (40)   Down arrow.


    F1              = 0x70,      // (112)
    F2              = 0x71,      // (113)
    F3              = 0x72,      // (114)
    F4              = 0x73,      // (115)
    F5              = 0x74,      // (116)
    F6              = 0x75,      // (117)
    F7              = 0x76,      // (118)
    F8              = 0x77,      // (119)
    F9              = 0x78,      // (120)
    F10             = 0x79,      // (121)
    F11             = 0x7A,      // (122)
    F12             = 0x7B,      // (123)
    F13             = 0x7C,      // (124)
    F14             = 0x7D,      // (125)
    F15             = 0x7E,      // (126)
    F16             = 0x7F,      // (127)
    F17             = 0x80,      // (128)
    F18             = 0x81,      // (129)
    F19             = 0x82,      // (130)
    F20             = 0x83,      // (131)
    F21             = 0x84,      // (132)
    F22             = 0x85,      // (133)
    F23             = 0x86,      // (134)
    F24             = 0x87,      // (135)
};

enum GLMouse
{
    LeftButton      = 0x01,      // (1)
    RightButton     = 0x02,      // (2)
    MidButton       = 0x04,      // (4)
};

class Safe_Thread
{
private:
	std::thread		m_thread;
	std::string		m_thread_name;

	bool			m_bcreated;
	bool			m_bdetach;
public:
	
	explicit Safe_Thread() :
		m_thread(),
		m_thread_name("none"),
		m_bcreated(false),
		m_bdetach(false)
	{

	}

	template<class ...Args>
	void create(Args &&...args)
	{
		m_thread =  std::move(std::thread(std::forward<Args>(args)...));
		m_bcreated = true;
	}

	Safe_Thread(Safe_Thread &&other) noexcept
	{
		m_thread = std::move(other.m_thread);
	}

	~Safe_Thread()
	{
		join();
	}

	bool is_created()
	{
		return m_bcreated;
	}

	void setname(const std::string& name)
	{
		m_thread_name = name;
	}

	void join()
	{
		if (m_thread.joinable())
			m_thread.join();
	}

	bool is_detach()
	{
		return m_bdetach;
	}

	void detach()
	{
		if (m_thread.joinable())
		{
			m_thread.detach();
			m_bdetach = true;
		}
	}
};


enum class FontWeight
{
	Normal,
	Bold,
	Thin,
	Light,
};

struct WndRender
{
	HDC		m_hDc;
	HGLRC	m_hGLRC;
};

struct WndProp
{
	bool                m_bFullScreen;
	bool                m_bGDIplus;
	bool                m_bOpenGL;       // State OpenGL
	int                 m_iAntialiasing; // Antialiasing level = 8
	int					m_iModeDraw;	 // 1 : use thread draw opengl | 0 :use pipe main thread
	bool				m_writeinfo;
	int					m_iAphaTrans = -1;


	DWORD               m_dwExStyle;     // Window Extended Style
	DWORD               m_dwStyle;       // Window Style


	void set_default()
	{
		m_bFullScreen   = false;
		m_bGDIplus	    = false;
		m_bOpenGL	    = false;
		m_iAntialiasing = -1;
		m_iModeDraw		= 0;
		m_writeinfo		= false;
	}

	WndProp()
	{
		this->set_default();
	}

	WndProp(const char* title, int xpos, int ypos, int width = 640, int height = 480)
	{
		m_bFullScreen = false;
		m_bGDIplus = false;
		m_bOpenGL = true;
		m_iAntialiasing = -1;
	}
};

struct WndStatus
{
	std::wstring        m_title;
	int                 m_x;
	int                 m_y;
	int                 m_width;
	int                 m_height;
	int                 m_RelState;

	WndStatus()
	{
		m_title = L"None";
		m_x = 0;
		m_y = 0;
		m_width = 640;
		m_height = 480;
		m_RelState = 0;    // Trạng thái mặc định
	}
};

//==================================================================================
// Class Window : Thông tin và ngữ cảnh của một handle                              
//==================================================================================
class Window
{
private:
	std::stack<WndProp>    m_PropStack;
	std::stack<WndStatus>  m_StatusStack;

	std::vector<Control*>  m_controls;

private:
	HWND                   m_hWnd;
	WndProp                m_pProp;
	WndRender              m_pRender;
	GDIplusToken           m_gdiToken;
	std::map<int, bool>    m_mouse;
	std::map<int, bool>    m_keyboard;
	MSG					   m_msg;
	bool				   m_bclosed;


	short                  m_zDeltaScroll;

private:
	bool				   m_bUseOpenGLEx;
private:
	// Window property
	std::wstring           m_title;
	int                    m_x;
	int                    m_y;
	int                    m_width;     // The drawable width
	int                    m_height;    // The drawable height

										// State information
	bool                   m_bShow;

	// System information : fps -frametime
	fox::CFPSCounter       m_fpscounter;

	// Text render 2D
	GLWinFontRender        m_text_render;
	const char*            m_fontNameTextRender;
	unsigned int           m_fontSizeTextRender;
	bool                   m_bSysInfo;

	// System draw infor
	bool				   m_bUpdateRenderInfo; // update view , text render info :
	int					   m_iIdUpdate;
	std::mutex						m_renderinfo_mutex;
	std::condition_variable			m_sycn_renderinfo;

	std::wstring		   m_gpu_device_name;

	// Control section
	unsigned int		   m_idsctrl;

	Safe_Thread			   m_drawthread;
	Safe_Thread			   m_processthread;

	// Font information
	std::wstring           m_fontName;
	unsigned int           m_fontSize;
	FontWeight             m_fontWeight;
private:
	void(*m_funOnDraw)         (Window* win) = NULL;
	void(*m_funOnCreated)      (Window* win) = NULL;
	void(*m_funOnDestroy)      (Window* win) = NULL;
	void(*m_funOnPaint)        (Window* win) = NULL;
	void(*m_funOnMouse)        (Window* win, int button, int action) = NULL;
	void(*m_funOnMouseRealt)   (Window* win) = NULL;
	void(*m_funOnMouseMove)    (Window* win) = NULL;
	void(*m_funOnKeyboard)     (Window* win) = NULL;
	void(*m_funOnProcess)      (Window* win) = NULL;
	void(*m_funOnResize)       (Window* win) = NULL;
	void(*m_funOnMouseScroll)  (Window* win) = NULL;

public:
	HWND GetHwnd() { return m_hWnd; }
	HDC  GetHDC() { return m_pRender.m_hDc; }
	// Get set don't care
	void SetOnDrawfunc			  (void(*funOnDraw)(Window*))		{ m_funOnDraw = funOnDraw;			}
	void SetOnCreatedfunc		  (void(*funOnCreate)(Window*))		{ m_funOnCreated = funOnCreate;		}
	void SetOnDestroyfunc		  (void(*funOnDestroy)(Window*))	{ m_funOnDestroy = funOnDestroy;	}
	void SetOnPaintfunc			  (void(*funOnPaint)(Window*))		{ m_funOnPaint = funOnPaint;		}
	void SetOnMouseButtonfunc	  (void(*funOnMouse)(Window*, int, int))		{ m_funOnMouse = funOnMouse;		}
	void SetOnMouseButtonRealtfunc(void(*funOnMouse)(Window*))		{ m_funOnMouseRealt = funOnMouse;	}
	void SetOnMouseMovefunc		  (void(*funOnMouseMove)(Window*))	{ m_funOnMouseMove = funOnMouseMove;}
	void SetOnKeyboardfunc		  (void(*funOnKeyboard)(Window*))	{ m_funOnKeyboard = funOnKeyboard;	}
	void SetProcessfunc			  (void(*funProcess)(Window*))		{ m_funOnProcess = funProcess;		}
	void SetOnResizefunc		  (void(*funOnResize)(Window*))		{ m_funOnResize = funOnResize;		}
	void SetOnMouseScrollfunc	  (void(*funOnScroll)(Window*))		{ m_funOnMouseScroll = funOnScroll; }

	//==================================================================================
	//⮟⮟ Triển khai hàm   - not important                                              
	//==================================================================================
	virtual void OnCreated()
	{
		if (m_funOnCreated)
			this->m_funOnCreated(this);
	}
	virtual void OnCommand(int type)
	{

	}

	virtual void OnKeyBoard()
	{
		if (m_funOnKeyboard)
			this->m_funOnKeyboard(this);
	}
	virtual void OnMouseButton(int button, int action)
	{
		if (m_funOnMouse)
			this->m_funOnMouse(this, button, action);
	}
	virtual void OnMouseMove()
	{
		if (m_funOnMouseMove)
			this->m_funOnMouseMove(this);
	}
	virtual void OnProcess()
	{
		if (m_funOnMouseRealt)
			this->m_funOnMouseRealt(this);

		if (m_funOnProcess)
			this->m_funOnProcess(this);
	}
	virtual void OnResize()
	{
		if (m_funOnResize)
			this->m_funOnResize(this);
	}
	virtual void OnMouseScroll()
	{
		if (m_funOnMouseScroll)
			this->m_funOnMouseScroll(this);
	}
	virtual void SwapBuffer()
	{
		SwapBuffers(m_pRender.m_hDc);
	}

	virtual void OnPaint()
	{
		if (m_funOnPaint)
			this->m_funOnPaint(this);
	}
	//==================================================================================
	//⮝⮝ Triển khai hàm  - not important                                               
	//==================================================================================
	static LRESULT CALLBACK WndDummyMainProc(HWND hWnd, UINT message, WPARAM wParam,
		LPARAM lParam)
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	static void paintControlBackground(HWND hwnd, HDC dc)
	{
		HWND parent;
		RECT r;
		POINT p;
		int saved;

		parent = GetParent(hwnd);
		if (parent == NULL)
			return;
		if (GetWindowRect(hwnd, &r) == 0)
			return;
		// the above is a window rect; convert to client rect
		p.x = r.left;
		p.y = r.top;
		if (ScreenToClient(parent, &p) == 0)
			return;
		saved = SaveDC(dc);
		if (saved == 0)
			return;
		if (SetWindowOrgEx(dc, p.x, p.y, NULL) == 0)
			return;
		SendMessageW(parent, WM_PRINTCLIENT, (WPARAM)dc, PRF_CLIENT);
		if (RestoreDC(dc, saved) == 0)
			return;
	}
	static LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Window* win = (Window*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
		switch (message)
		{
		case WM_CLOSE:
		{
			win->m_bclosed = true;
			break;
		}
		case WM_DESTROY:
		{
			win->m_bclosed = true;
			win->OnDestroy();
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			win->SetKeyboardStatus((int)wParam, true);
			win->OnKeyBoard();
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			win->SetKeyboardStatus((int)wParam, false);
			win->OnKeyBoard();
			break;
		}
		case WM_LBUTTONUP:
		{
			win->SetMouseButtonStatus(VK_LBUTTON, false);
			win->OnMouseButton(GLMouse::LeftButton, GL_RELEASE);
			break;
		}
		case WM_RBUTTONUP:
		{
			win->SetMouseButtonStatus(VK_RBUTTON, false);
			win->OnMouseButton(GLMouse::RightButton, GL_RELEASE);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			win->SetMouseButtonStatus(VK_LBUTTON, true);
			win->OnMouseButton(GLMouse::LeftButton, GL_PRESSED);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			win->SetMouseButtonStatus(VK_RBUTTON, true);
			win->OnMouseButton(GLMouse::RightButton, GL_PRESSED);
			break;
		}
		case WM_MOUSEMOVE:
		{
			win->OnMouseMove();
			break;
		}
		case WM_SIZE: //Check if the window has been resized
		{
			win->m_width  = LOWORD(lParam); // width
			win->m_height = HIWORD(lParam); // height
			win->UpdateRenderInfo();

			win->UpdateTitle();
			// cannot use opengl context in this tunnel
			win->OnResize();
			
			// Refresh screen when resize window in case one thread
			if (win->GetDrawMode() == 0)
				win->OnDraw();

			break;
		}
		case WM_SIZING:
		{
			win->UpdateTitle();
			break;
		}
		case WM_MOUSEWHEEL:
		{
			win->m_zDeltaScroll = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
			win->OnMouseScroll();
			break;
		}
		case WM_COMMAND:
		{
			WORD wID  = LOWORD(wParam); // item, control, or accelerator identifier
			WORD wEvt = HIWORD(wParam); // item event
			HWND hwndControl = (HWND)lParam;  // handle of control
			if (hwndControl)
			{
				Control* ctrl = (Control*)(GetWindowLongPtr(hwndControl, GWLP_USERDATA));
				if (ctrl) ctrl->Event(win, wID, wEvt);
			}
			break;
		}
		case WM_DRAWITEM:
		{
			WORD wID = LOWORD(wParam); // item, control, or accelerator identifier
			LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
			Control* ctrl = (Control*)(GetWindowLongPtr(pdis->hwndItem, GWLP_USERDATA));
			if (ctrl)
				ctrl->Draw(pdis);
			break;
		}
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN: //In order to make those edges invisble when we use RoundRect(),
		{                //we make the color of our button's background match window's background
			SetBkMode((HDC)wParam, TRANSPARENT);
			HWND hwnd = (HWND)lParam;

			Control* ctrl = (Control*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (ctrl && ctrl->GetType() == CtrlType::LABEL)
			{
				Label* lab = static_cast<Label*>(ctrl);
				lab->UpdateTextColor((HDC)wParam);
			}

			return (INT_PTR)GetStockObject((HOLLOW_BRUSH));
		}
		case WM_PAINT:
		{
			win->OnPaint();
			break;  //[BUG] always drawing
		}
		case WM_ERASEBKGND:
		{

			return TRUE;
		}
		default:
		{
			break;
		}
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

private:
	// get gpu name use opengl 
	virtual std::wstring GetGPUDevice()
	{
		std::string gpu_device = "No Device";
		if ((char*)glGetString(GL_RENDERER))
		{
			gpu_device = (char*)glGetString(GL_RENDERER);
		}
		return fox::from_utf8(gpu_device);
	}

private:
	virtual void OnDraw()
	{
		if (!this->MakeContext()) return;

		if (m_pProp.m_iModeDraw == 1)
		{
			if (m_bUpdateRenderInfo)
			{
				glViewport(0, 0, m_width, m_height);
				UpdateTextRender();

				m_gpu_device_name = this->GetGPUDevice();

				m_bUpdateRenderInfo = false;
				m_sycn_renderinfo.notify_all();
			}
		}
		else
		{
			glViewport(0, 0, m_width, m_height);
			UpdateTextRender();
		}

		this->UpdateWndInfo();

		// Thực hiện vẽ custom người dùng
		if (m_funOnDraw)
		{
			//glViewport(0, 0, m_width, m_height);
			this->m_funOnDraw(this);
		}

		// Hiển thị thông tin fps - frametime và thông tin hệ thống nếu cần
		if (m_bSysInfo == true)
		{
			char strsys[80];
			sprintf_s(strsys, "fps: %d - frametime : %f ms", GetFPS(), GetFrameTime());
			this->WriteText(strsys, 10, 20);
		}

		this->SwapBuffer();
	}

private:

	void SetMouseButtonStatus(int btn, bool status)
	{
		m_mouse[btn] = status;
	}
	void SetKeyboardStatus(int key, bool status)
	{
		m_keyboard[key] = status;
	}
	//======================================================================================
	//⮟⮟ Triển khai window win32                                                           
	//======================================================================================

private:
	static bool CALLBACK SetChildFont(HWND hwnd, LPARAM font)
	{
		HFONT hFont = (HFONT)SendMessage(hwnd, WM_GETFONT, NULL, NULL);
		if (!hFont)
		{
			SendMessage(hwnd, WM_SETFONT, font, TRUE);
		}
		return TRUE;
	}
	//==================================================================================
	// Cập nhật thông font cho window                                                   
	//==================================================================================
	void UpdateFont()
	{
		if (!m_hWnd || m_fontName.empty()) return;

		int iFontWeight = FW_NORMAL;
		switch (m_fontWeight)
		{
		case FontWeight::Bold: iFontWeight = FW_BOLD; break;
		case FontWeight::Thin: iFontWeight = FW_THIN; break;
		case FontWeight::Light: iFontWeight = FW_LIGHT; break;
		default:
			break;
		}
		HFONT hFont = NULL;
		hFont = CreateFont(m_fontSize, 0, 0, 0, iFontWeight, 0,
				0, 0, 0, 0, 0, 0, 0, m_fontName.c_str());

		SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, FALSE);

		EnumChildWindows(m_hWnd, (WNDENUMPROC)SetChildFont, (LPARAM)hFont);
	}

public:
	void SetFont(const wchar_t* fontName, const unsigned int fontSize, const FontWeight fontWeight = FontWeight::Normal)
	{
		m_fontName = fontName;
		m_fontSize = fontSize;
		m_fontWeight = fontWeight;

		UpdateFont();
	}

	//======================================================================================
	//⮟⮟ Triển khai chính khởi tạo và xử lý control                                        
	//======================================================================================
private:
	//==================================================================================
	// Khởi tạo toàn bộ control đã được thêm                                            
	//==================================================================================
	virtual void OnInitControl()
	{
		for (int i = 0; i < m_controls.size(); i++)
		{
			if (!m_controls[i]->IsCreated())
			{
				m_controls[i]->SetParent(m_hWnd);
				m_controls[i]->OnInitControl(m_idsctrl);
			}
		}
	}

	//==================================================================================
	// Xóa và hủy tòa bộ control có trong window                                        
	//==================================================================================
	virtual void DestroyControl()
	{
		for (int i = 0; i < m_controls.size(); i++)
		{
			m_controls[i]->OnDestroy();
			delete m_controls[i];
		}
		m_controls.clear();
	}

	//==================================================================================
	// Xử lý sự kiện khi một Control được kích hoạt sự kiện                             
	//==================================================================================
	//void OnCommandControl(HWND hwndCtrl, WORD ID, WORD Event)
	//{
	//	Control * control = GetControlFormID(ID);
	//	if (!control) return;
	//	control->Event(this, ID, Event);
	//}

	//void OnDrawControl(HWND hwndCtrl, WORD ID)
	//{
	//	Control * control = GetControlFormID(ID);
	//	if (!control) return;
	//	control->Draw();
	//}

	//==================================================================================
	// Lấy control từ ID của nó                                                         
	//==================================================================================
	Control* GetControlFormID(WORD ID)
	{
		for (int i = 0; i < m_controls.size(); i++)
		{
			INT CtrlID = m_controls[i]->GetID();

			if (CtrlID == ID || m_controls[i]->ContainID(ID))
			{
				return m_controls[i];
			}
		}
		return NULL;
	}

	//======================================================================================
	//⮟⮟ Triển khai chính tạo và xử lý ngữ cảnh window  - important                        
	//======================================================================================
private:

	//==================================================================================
	// Lấy mảng thuộc tính thiết lập để set style window                                
	//==================================================================================
	void GetFixelFormatAttribute(int *attribs, int attribsize)
	{
		int attribCount = 0;

		addAtribute(attribs, WGL_DRAW_TO_WINDOW_ARB, GL_TRUE);
		addAtribute(attribs, WGL_SUPPORT_OPENGL_ARB, GL_TRUE);
		addAtribute(attribs, WGL_DOUBLE_BUFFER_ARB, GL_TRUE);
		addAtribute(attribs, WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB);
		addAtribute(attribs, WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB);
		addAtribute(attribs, WGL_COLOR_BITS_ARB, 32);
		addAtribute(attribs, WGL_DEPTH_BITS_ARB, 24);
		addAtribute(attribs, WGL_STENCIL_BITS_ARB, 8);

		if (m_pProp.m_iAntialiasing > 0)
		{
			addAtribute(attribs, WGL_SAMPLE_BUFFERS_ARB, GL_TRUE);          // Enable multisampling
			addAtribute(attribs, WGL_SAMPLES_ARB, m_pProp.m_iAntialiasing); // Number of samples
		}
		addAtributeEnd(attribs);
	}

	//==================================================================================
	// Khởi tạo ngữ cảnh OpenGL (OpenGL context)                                        
	// bOpenGLex : sử dụng OpenGL mở rộng                                               
	//==================================================================================
	bool CreateOpenGLContext(/*bool bOpenGLEx = true*/)
	{
		HDC   hDC   = GetDC(m_hWnd);
		HGLRC hglrc = NULL;

		int iPixelFormat; unsigned int num_formats = 0;

		// Get pixel format attributes through "modern" extension
		if (m_bUseOpenGLEx)
		{
			int pixelAttribs[47];
			GetFixelFormatAttribute(pixelAttribs, sizeof(pixelAttribs) / sizeof(pixelAttribs[0]));

			wglChoosePixelFormatARB(hDC, pixelAttribs, 0, 1, &iPixelFormat, &num_formats);

			PIXELFORMATDESCRIPTOR pfd;
			DescribePixelFormat(hDC, iPixelFormat, sizeof(pfd), &pfd);
			SetPixelFormat(hDC, iPixelFormat, &pfd);

			// Specify that we want to create an OpenGL x.x core profile context
			int gl_attribs[] = {
				WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
				WGL_CONTEXT_MINOR_VERSION_ARB, 5,
				WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
				0,
			};
			hglrc = wglCreateContextAttribsARB(hDC, 0, gl_attribs);
		}
		// Get pixel format attributes through legacy PFDs
		else
		{
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};
			iPixelFormat = ChoosePixelFormat(hDC, &pfd);
			SetPixelFormat(hDC, iPixelFormat, &pfd);

			hglrc = wglCreateContext(hDC);
		}

		if (wglMakeCurrent(hDC, hglrc))
		{
			m_pRender.m_hDc = hDC;
			m_pRender.m_hGLRC = hglrc;
			m_pProp.m_bOpenGL = true;
			return true;
		}
		else
		{
			DeleteOpenGLContext();
			m_pProp.m_bOpenGL = false;
			return false;
		}
	}

public:
	/***************************************************************************
	*! @brief  : register window class use window context
	*! @author : thuong.nv - [Date] : 08/10/2022
	*! @param    [In] strClassName : Name class
	*! @param    [In] Proc		   : function hanle event
	*! @param    [In] hInst		   : default GetModuleHandle(NULL)
	*! @return : 1 : OK , 0 : False
	*! @note   : N/A
	***************************************************************************/
	static int register_window_class(const wchar_t* strClassName, WNDPROC Proc, HINSTANCE hInst)
	{
		WNDCLASSEXW  wClass;
		ZeroMemory(&wClass, sizeof(WNDCLASSEX));
		wClass.cbClsExtra = NULL;
		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.cbWndExtra = NULL;
		wClass.hbrBackground = (HBRUSH)(COLOR_MENU);
		wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wClass.hIconSm = NULL;
		wClass.hInstance = hInst;
		wClass.lpfnWndProc = (WNDPROC)Proc;
		wClass.lpszClassName = strClassName;
		wClass.lpszMenuName = NULL;
		wClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		if (!RegisterClassEx(&wClass))
			return 0;
		return 1;
	}


	/***************************************************************************
	*! @brief  : load extension opengl library glew
	*! @author : thuong.nv - [Date] : 08/10/2022
	*! @param    [In] strClassName : Name class
	*! @param    [In] Proc		   : function hanle event
	*! @param    [In] hInst		   : default GetModuleHandle(NULL)
	*! @return : 1: OK | 0 False;
	*! @note   : N/A
	***************************************************************************/
	static int load_opengl_extension()
	{
		int bInitOk = FALSE;
		if (register_window_class(L"DummyClass", WndDummyMainProc, GetModuleHandle(NULL)))
		{
			HWND hWnd_dummy = CreateWindowEx(
				0, L"DummyClass", L"Dummy OpenGL Window", 0,
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				0, 0, NULL, 0);

			PIXELFORMATDESCRIPTOR pixelFormat = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC hDC_dummy = GetDC(hWnd_dummy);
			int iPixelFormat = ChoosePixelFormat(hDC_dummy, &pixelFormat);
			SetPixelFormat(hDC_dummy, iPixelFormat, &pixelFormat);

			HGLRC hglrc_dummy = wglCreateContext(hDC_dummy);
			if (wglMakeCurrent(hDC_dummy, hglrc_dummy)) {

				if (glewInit() != GLEW_OK)  // Load library OpenGL extension
					bInitOk = FALSE;
				else
					bInitOk = TRUE;
			}

			wglMakeCurrent(hDC_dummy, 0);
			wglDeleteContext(hglrc_dummy);
			ReleaseDC(hWnd_dummy, hDC_dummy);
			DestroyWindow(hWnd_dummy);
		}

		return bInitOk;
	}

private:
	//==================================================================================
	// Xóa ngữ cảnh OpenGL trên window                                                  
	//==================================================================================
	void DeleteOpenGLContext()
	{
		ReleaseDC(m_hWnd, m_pRender.m_hDc); // release device context
		wglDeleteContext(m_pRender.m_hGLRC);// delete the rendering context
		m_pRender.m_hDc = NULL;
		m_pRender.m_hGLRC = NULL;
	}

	//==================================================================================
	// Khởi tạo window sử dụng GDI plus                                                 
	//==================================================================================
	void CreateGDIplus()
	{
		if (m_pProp.m_bGDIplus)
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR           gdiplusToken;

			Gdiplus::Status status = Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
			if (status == Gdiplus::Status::Ok)
			{
				m_gdiToken.m_id = gdiplusToken;
				m_gdiToken.m_prop = gdiplusStartupInput;
			}
			else
			{
				m_gdiToken.m_id = NULL;
			}
		}
	}

	//==================================================================================
	// Xóa ngữ cảnh window sử dụng GDI plus                                             
	//==================================================================================
	void DeleteGDIplus()
	{
		Gdiplus::GdiplusShutdown(m_gdiToken.m_id);
	}

	//==================================================================================
	// Hàm tạo handle và thông số                                                       
	//==================================================================================
	bool _CreateWindow(const wchar_t* strWndClassName)
	{
		// Create GDI+ startup incantation
		this->CreateGDIplus();

		// Kích thức thực tế của vùng có thể vẽ
		RECT wr = { 0, 0, m_width, m_height };     // set the size, but not the position
		AdjustWindowRect(&wr, m_pProp.m_dwStyle, FALSE);// adjust the size

		m_hWnd = CreateWindowEx(       //
			m_pProp.m_dwExStyle,        //
			strWndClassName,            //
			m_title.c_str(),            //
			m_pProp.m_dwStyle,          //
			m_x, m_y,                   // Postion 
			wr.right - wr.left,         // Actual width size
			wr.bottom - wr.top,         // Actual height size
			NULL,                       //
			NULL,                       //
			NULL,                       //
			NULL                        //
		);

		// Create window failed or associate failed
		if (!m_hWnd)
		{
			this->DeleteGDIplus();
			::DestroyWindow(m_hWnd);
			return false;
		}
		SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

		// Update size window after created
		RECT rect;
		if (GetClientRect(m_hWnd, &rect))
		{
			m_width = rect.right - rect.left;
			m_height = rect.bottom - rect.top;
		}
		return true;
	}

	void SetUseOpenGLExtension(const bool& buse)
	{
		this->m_bUseOpenGLEx = buse;
	}

	//==================================================================================
	// Khởi tạo window và thiết lập thông số                                            
	//==================================================================================
	bool _OnCreate(const wchar_t* strClassname)
	{
		bool ret = true;
		// Update get style window
		this->SetUpHint();

		// Create a window HWND use class name
		ret &= this->_CreateWindow(strClassname);

		if (ret) // it OK
		{
			// Update get style window
			this->UpdateHint();

			// Active function user custom
			this->OnCreated();

			// Update title after created ok
			this->UpdateTitle();

			// Initialization control
			this->OnInitControl();

			// Update font control after initialization control
			this->UpdateFont();

			// Update and setup properties when everything is done
			this->InitProperties();
		}

		return ret;
	}

	//==================================================================================
	// Khởi tạo window và thiết lập thông số                                            
	//==================================================================================
	bool _OnCreateOpenGLContext(bool bInitOpenGLEx = false)
	{
		// Set up use OpenGL extension
		this->SetUseOpenGLExtension(bInitOpenGLEx);

		// Any given OpenGL rendering context can be active at only one thread at a time.
		// if I create opengl context in main thread then this thread not active
		//this->CreateOpenGLContext();

		if (m_pProp.m_iModeDraw == 1)
		{
			if (!m_drawthread.is_created())
			{
				m_drawthread.create(&Window::ThreadDrawOpenGL, this);
				m_drawthread.detach();
			}

			if (!m_processthread.is_created())
			{
				m_processthread.create(&Window::ThreadProcessOpenGL, this);
			}
		}
		// create opengl main thread
		else
		{
			auto ret = this->CreateOpenGLContext();
			if(ret)
				this->ReloadTextRender();
			return ret;
		}

		return true;
	}

	//=======================================================================================
	//⮟⮟ Triển khai cập nhật trạng thái của window                                          
	//=======================================================================================
private:
	//===================================================================================
	// Lưu giữ trạng thái thông tin của window                                           
	//===================================================================================
	void PushWindowStatus()
	{
		WndStatus status;
		status.m_title = m_title;
		status.m_x = m_x;
		status.m_y = m_y;
		status.m_width = m_width;
		status.m_height = m_height;
		status.m_RelState = 1;

		m_StatusStack.push(status);
	}

	//===================================================================================
	// Lấy lại trạng thái đã được lưu dữ trước đó                                        
	//===================================================================================
	WndStatus PopWindowStatus()
	{
		WndStatus status;
		if (!m_StatusStack.empty())
		{
			status = m_StatusStack.top();
			m_StatusStack.pop();
		}
		return status;
	}

	//===================================================================================
	// Cập nhật trạng thái thời gian mỗi khi một frame trôi qua                          
	//===================================================================================
	void UpdateWndInfo()
	{
		m_fpscounter.update();
	}

	//===================================================================================
	// Khởi tạo trạng thái và thông tin thuộc tính khi đã tạo windows xong               
	//===================================================================================
	void InitProperties()
	{
		m_fpscounter.start();
	}

	//===================================================================================
	// send notify to draw thread and update render info
	//===================================================================================
	void UpdateRenderInfo()
	{
		if (m_pProp.m_iModeDraw == 1)
		{
			// update information draw in thread draw and notify ok to main thread
			m_bUpdateRenderInfo = true;
			std::unique_lock< std::mutex> lock(m_renderinfo_mutex);

			// wait for : draw thread update draw info ok
			while (m_bUpdateRenderInfo && m_drawthread.is_detach())
			{
				m_sycn_renderinfo.wait(lock);
			}
		}
		else
		{
			
		}
	}

	//===================================================================================
	// Cập nhật lại title window                                                         
	//===================================================================================
	void UpdateTitle()
	{
		if (!m_hWnd) return;

		wchar_t titlebuff[256];

		std::wstring gpu_name = L"Unknown";

		if (m_pProp.m_iModeDraw == 1)
			gpu_name = m_gpu_device_name.c_str();
		else
			gpu_name = this->GetGPUDevice();

		// m_gpu_deive_name updated in render thread
		swprintf_s(titlebuff, L"%s - %d x %d - %s",  m_title.c_str(),
			m_width, m_height,
			gpu_name.c_str());
		SetWindowText(m_hWnd, titlebuff);
	}

	//===================================================================================
	// Cập nhật lại thông số cho text render window                                      
	//===================================================================================
	void UpdateTextRender()
	{
		m_text_render.UpdateView(m_width, m_height);
	}

	//===================================================================================
	// Load và cập nhật lại text render trên window                                      
	//===================================================================================
	void ReloadTextRender()
	{
		// Setup Text render;
		m_text_render.Init(this->GetHDC(), m_width, m_height);
		m_text_render.LoadFont(m_fontNameTextRender, m_fontSizeTextRender);
	}

	//===================================================================================
	// Cập nhật trạng thái hiển thị                                                      
	//===================================================================================
	void UpdateState(bool bShow = true)
	{
		if (!m_hWnd) return;

		if (bShow)
		{
			ShowWindow(this->m_hWnd, SW_SHOW);
		}
		else
		{
			ShowWindow(this->m_hWnd, SW_HIDE);
		}
	}

	//===================================================================================
	// Cập nhật thông tin stype của window                                               
	//===================================================================================
	void UpdateHint()
	{
		if (m_pProp.m_bFullScreen)
		{
			this->PushWindowStatus();

			// Set new window style and size.
			SetWindowLong(m_hWnd, GWL_STYLE, m_pProp.m_dwStyle);
			SetWindowLong(m_hWnd, GWL_EXSTYLE, m_pProp.m_dwExStyle);

			// On expand, if we're given a window_rect, grow to it, otherwise do not resize.
			xMonitorInfo monitor = fox::get_monitorinfo();
			SetWindowPos(m_hWnd, NULL, 0, 0, monitor.WIDTH, monitor.HEIGHT, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			// Update width ,height screen
			m_width = monitor.WIDTH;
			m_height = monitor.HEIGHT;
		}
		else
		{
			WndStatus winState = this->PopWindowStatus();
			if (winState.m_RelState)
			{
				// Cập nhật lại style của Window;
				this->UpdateStyleWindow();

				if (ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL)
				{
					SetWindowLong(m_hWnd, GWL_STYLE, m_pProp.m_dwStyle);
					SetWindowLong(m_hWnd, GWL_EXSTYLE, m_pProp.m_dwExStyle);
					m_x = winState.m_x;
					m_y = winState.m_y;
					m_width = winState.m_width;
					m_height = winState.m_height;

					RECT wr = { 0, 0, m_width, m_height };           // set the size, but not the position
					AdjustWindowRect(&wr, m_pProp.m_dwStyle, FALSE); // adjust the size

					SetWindowPos(m_hWnd, HWND_NOTOPMOST, m_x, m_y, (wr.right - wr.left), (wr.bottom - wr.top), SWP_SHOWWINDOW);

					// Update size window after created
					m_width = winState.m_width;
					m_height = winState.m_height;
				}
			}
		}

		if (m_pProp.m_iAphaTrans >= 0)
		{
			SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
			SetLayeredWindowAttributes(m_hWnd, RGB(0, 0, 0), m_pProp.m_iAphaTrans, LWA_ALPHA);
		}
	}
	//======================================================================================
	//⮟⮟ Triển khai chức năng hỗ trợ cho window                                            
	//======================================================================================
public:

	//===================================================================================
	// Hiển thị text ra window                                                           
	//===================================================================================
	void WriteText(const char* text, int x, int y, float r = 1.0f, float g = 1.0f,
		float b = 1.0f, float a = 1.0f)
	{
		m_text_render.Use();
		m_text_render.Write(x, y, text, r, g, b, a);
		m_text_render.DontUse();
	}

	//===================================================================================
	// Có hiển thị thông tin hệ thống lên window hay không                               
	//===================================================================================
	void WriteSystemInfo(bool bShow = true)
	{
		m_bSysInfo = bShow;
	}

public:
	const Window& operator=(const Window& win) = delete;

	Window(	const wchar_t* title, const int& xpos, const int& ypos,
			const int& width = 640, const int height = 480,
			const WndProp* prop = NULL) : m_hWnd(NULL)
	{
		this->m_title  = title;
		this->m_x	   = xpos;
		this->m_y	   = ypos;
		this->m_width  = width;
		this->m_height = height;

		// setup default font
		this->m_fontName = L"Arial";
		this->m_fontSize = 12;
		this->m_fontWeight = FontWeight::Normal;

		// openGL information
		this->m_bUseOpenGLEx = false; // it reup when create window and load opengl extension
		this->m_bUpdateRenderInfo = false;
		this->m_iIdUpdate = 0;

		// control setup
		this->m_idsctrl = 1000;

		// setup default text render
		this->m_fontNameTextRender = "Consolas";
		this->m_fontSizeTextRender = 16;

		this->m_bSysInfo = false;
		this->m_bclosed  = false;

		if (prop)
			SetPropAdvanced(*prop);
	}

	void SetPropAdvanced(const WndProp& prop)
	{
		this->m_pProp = prop;
	}

	void UpdateStyleWindow()
	{
		m_pProp.m_dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE ;     // Window Extended Style
		m_pProp.m_dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_EX_TRANSPARENT;  // Windows Style
																	//@@ WS_CLIPCHILDREN: Control của window sẽ không được vẽ khi SwapBuffer

		if (m_pProp.m_bFullScreen)                           // Are We Still In Fullscreen Mode?
		{
			// Window Extended Style
			m_pProp.m_dwExStyle = m_pProp.m_dwExStyle &~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
			// Windows Style
			m_pProp.m_dwStyle = m_pProp.m_dwStyle &~(WS_CAPTION | WS_THICKFRAME);
		}
	}

	void SetUpHint()
	{
		this->UpdateStyleWindow();
	}

	void ExitFullScreen()
	{
		m_pProp.m_bFullScreen = false;
		this->UpdateHint();
	}

	void FullScreen()
	{
		m_pProp.m_bFullScreen = true;
		this->UpdateHint();
	}

	void Show()
	{
		this->m_bShow = true;
		this->UpdateState(m_bShow);
	}

	void Hide()
	{
		this->m_bShow = false;
		this->UpdateState(m_bShow);
	}

	void SetTitle(std::wstring title)
	{
		m_title = title;
		this->UpdateTitle();
	}

	// Activate openGL context
	bool MakeContext()
	{
		if (m_pRender.m_hDc && m_pRender.m_hGLRC)
		{
			if (wglMakeCurrent(m_pRender.m_hDc, m_pRender.m_hGLRC))
			{
				return true;
			}
		}
		//assert(0);
		return false;
	}

	// @return : 0 : false, 1 ok
	int AddControl(Control* control)
	{
		if (!control)
			return 0;

		if (m_hWnd == NULL)
		{
			// it will initialize later
			m_controls.push_back(control);
		}
		else
		{
			// if existed window handle then initiation control
			control->SetParent(m_hWnd);
			if (control->OnInitControl(m_idsctrl))
			{
				m_controls.push_back(control);
			}
			else
			{
				return 0;
			}
		}
		return 1;
	}

	int       GetDrawMode() { return m_pProp.m_iModeDraw; }
	int       GetWidth() { return m_width; }
	int       GetHeight() { return m_height; }
	int       GetMouseScroll() { return (int)m_zDeltaScroll; }

	//==================================================================================
	// Lấy vị trí tương đối của con trỏ so điểm gốc (trái trên)                         
	//==================================================================================
	bool GetCursorPos(int& xpos, int& ypos)
	{
		POINT cursor_pos;
		if (::GetCursorPos(&cursor_pos) && ScreenToClient(m_hWnd, &cursor_pos))
		{
			xpos = cursor_pos.x;
			ypos = cursor_pos.y;
			return true;
		}
		else
		{
			xpos = 0;
			ypos = 0;
			return false;
		}
	}
	bool GetCursorPos(float& xpos, float& ypos)
	{
		POINT cursor_pos;
		if (::GetCursorPos(&cursor_pos) && ::ScreenToClient(m_hWnd, &cursor_pos))
		{
			xpos = (float)cursor_pos.x;
			ypos = (float)cursor_pos.y;
			return true;
		}
		else
		{
			xpos = 0;
			ypos = 0;
			return false;
		}
	}

	POINT GetCursorPos()
	{
		POINT cursor_pos{ 0,0 };

		if (::GetCursorPos(&cursor_pos) && ::ScreenToClient(m_hWnd, &cursor_pos))
		{
			return cursor_pos;
		}
		return cursor_pos;
	}

	//==================================================================================
	// Lấy vị trí tương đối của con trỏ so với điểm giữa handle                         
	//==================================================================================
	bool GetCursorPosCenter(float& xpos, float& ypos)
	{
		POINT cursor_pos;
		if (::GetCursorPos(&cursor_pos) && ScreenToClient(m_hWnd, &cursor_pos))
		{
			xpos = -(m_width / 2 - (float)cursor_pos.x);
			ypos = (m_height / 2 - (float)cursor_pos.y);
			return true;
		}
		else
		{
			xpos = 0;
			ypos = 0;
			return false;
		}
	}

	//==================================================================================
	// Sử dụng cho trường hợp dùng GDIplus (xóa sau khi sử dụng xong)                   
	//==================================================================================
	WndGDIplus* GetGraphicsFromHWND()
	{
		return Gdiplus::Graphics::FromHWND(m_hWnd);;
	}

	//==================================================================================
	// Kiểm tra trạng thái key, key sẽ lấy ở GLWinDef                                   
	//==================================================================================
	bool GetKeyboardStatus(int keyboard)
	{
		return m_keyboard[keyboard];
	}

	//==================================================================================
	// Kiểm tra trạng thái button mouse, mouse sẽ lấy ở GLWinDef                        
	//==================================================================================
	bool GetMouseButtonStatus(int btn)
	{
		return m_mouse[btn];
	}

	//==================================================================================
	// Lấy thời gian trôi qua từ frame trước sang frame hiện tại                        
	//==================================================================================
	double GetFrameTime()
	{
		return m_fpscounter.frametime();
	}

	//==================================================================================
	// Lấy FPS window                                                                   
	//==================================================================================
	unsigned int GetFPS()
	{
		return m_fpscounter.fps();
	}

	//==================================================================================
	// Hàm hủy Window                                                                   
	//==================================================================================
	virtual void OnDestroy()
	{
		// Gọi hàm mở rộng trước
		if (m_funOnDestroy)
		{
			m_funOnDestroy(this);
		}

		// Xử lý hủy mặc định
		DeleteGDIplus();
		DeleteOpenGLContext();
		DestroyControl();       //N.V.Thuong 22.04.2022
	}

private:

	virtual void ThreadDrawOpenGL()
	{
		// if I create opengl context in main thread then this thread not active
		this->CreateOpenGLContext();

		// Setup Text render;
		this->ReloadTextRender();
		
		while (!this->closed())
		{
			this->OnDraw();
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	virtual void ThreadProcessOpenGL()
	{
		while (!this->closed())
		{
			this->OnProcess();
		}
	}

public:

	// function draw opengl : makecontext + swapbuffer
	void draw()
	{
		if (m_pProp.m_iModeDraw == 1)
		{
			if (!m_drawthread.is_detach())
			{
				m_drawthread.detach();
			}
		}
		else
		{
			this->OnDraw();
		}
	}

	void process()
	{
		if (m_pProp.m_iModeDraw == 1)
		{
			if (!m_processthread.is_detach())
			{
				m_processthread.detach();
			}
		}
		else
		{
			this->OnProcess();
		}
	}

	void poll_event()
	{
		// Xử lý trong trường hợp vẽ liên tục
		// Check to see if any messages are waiting in the queue
		while (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
		{
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
	}

	void wait_event()
	{
		// Xử lý trong trường hợp vẽ theo sự kiện
		if (GetMessage(&m_msg, NULL, 0, 0) > 0)
		{
			TranslateMessage(&m_msg);
			DispatchMessage(&m_msg);
		}
	}

	bool closed()
	{
		return m_bclosed;
	}
	void close()
	{
		PostMessage(m_hWnd, WM_CLOSE, NULL, NULL);
	}

	friend Window* fox_create_window(const wchar_t* , int , int , int , int, const WndProp*);
	friend Window* fox_create_window(Window*);
};


Window* fox_create_window(const wchar_t* title, int xpos, int ypos, int width = 640, int height = 480, const WndProp* prop = NULL)
{
	static int bRegWinClass   = FALSE;
	static int bInitOpenGLex  = FALSE;

	if (!bRegWinClass)
		bRegWinClass = Window::register_window_class(GL_WIN_CLASS, Window::WndMainProc, GetModuleHandle(NULL));

	if (bRegWinClass && !bInitOpenGLex)
		bInitOpenGLex = Window::load_opengl_extension();

	if (!bRegWinClass)
		return NULL;
	
	// create window handle
	Window* win = new Window(title, xpos, ypos, width, height, prop);
	if(!win->_OnCreate(GL_WIN_CLASS))
	{
		delete win;
		return NULL;
	}
	
	// create opengl context 
	if (!win->_OnCreateOpenGLContext(bInitOpenGLex))
	{
		win->close();
		delete win;
		return NULL;
	}

	win->Show();

	return win;
}

Window* fox_create_window(Window* win)
{
	static int bRegWinClass = FALSE;
	static int bInitOpenGLex = FALSE;

	if (!bRegWinClass)
		bRegWinClass = Window::register_window_class(GL_WIN_CLASS, Window::WndMainProc, GetModuleHandle(NULL));

	if (bRegWinClass && !bInitOpenGLex)
		bInitOpenGLex = Window::load_opengl_extension();

	if (!bRegWinClass)
		return NULL;

	if (!win->_OnCreate(GL_WIN_CLASS))
	{
		delete win;
		return NULL;
	}

	// create opengl context 
	if (!win->_OnCreateOpenGLContext(bInitOpenGLex))
	{
		win->close();
		delete win;
		return NULL;
	}

	win->Show();

	return win;
}

void fox_destroy_window(Window* win)
{
	delete win;
}

//
//class WindowManager
//{
//private:
//	unordered_map<HWND, Window*>            windows;
//	unordered_map<GLWindow*, vector<Window*>>    windowGLs;
//
//public:
//
//	//==================================================================================
//	// Hàm quản lý window chung                                                         
//	//==================================================================================
//	Window* Find(HWND hwnd)
//	{
//		auto found = windows.find(hwnd);
//		if (found != windows.end())
//		{
//			return found->second;
//		}
//		return NULL;
//	}
//
//	bool Add(Window* win)
//	{
//		if (!win || !win->m_hWnd) return false;
//
//		auto found = windows.find(win->m_hWnd);
//		if (found == windows.end())
//		{
//			windows[win->m_hWnd] = win;
//			windowGLs[win->m_pGLWin].push_back(win);
//			return true;
//		}
//		return false;
//	}
//
//	void Remove(HWND hwnd)
//	{
//		auto found = windows.find(hwnd);
//		if (found != windows.end())
//		{
//			Window* win = found->second;
//
//			// Xóa window mà gl quản lý
//			for (auto it = windowGLs[win->m_pGLWin].begin();
//				it != windowGLs[win->m_pGLWin].end();)
//			{
//				if (win == *it)
//				{
//					it = windowGLs[win->m_pGLWin].erase(it);
//				}
//				else
//				{
//					++it;
//				}
//			}
//			windows.erase(found);
//		}
//	}
//
//	int GetSize()
//	{
//		return (int)windows.size();
//	}
//
//	void Clear()
//	{
//		windows.clear();
//		windowGLs.clear();
//	}
//
//	//==================================================================================
//	// Hàm quản lý theo windowGL                                                        
//	//==================================================================================
//	bool winGL_IsMain(GLWindow* winGL, const Window* win)
//	{
//		if (windowGLs[winGL].empty()) return false;
//
//		if (windowGLs[winGL].front() == win)
//		{
//			return true;
//		}
//		return false;
//	}
//
//	Window* winGL_GetFront(GLWindow* winGL)
//	{
//		if (windowGLs[winGL].empty()) return NULL;
//
//		return windowGLs[winGL].front();
//	}
//
//	// Trả về tham chiếu đến danh sách của GLWindow
//	vector<Window*>& winGL_GetListWindow(GLWindow* pGL)
//	{
//		return windowGLs[pGL];
//	}
//};
//
//class GLWindow
//{
//private:
//	MSG                 m_msg;
//	Window*             m_pMainWin;
//
//	bool                bClosed = false;
//
//public:
//
//	static WindowManager* GetManaIntance()
//	{
//		static WindowManager winmana;
//		return &winmana;
//	}
//
//public:
//
//	static LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//	{
//		WindowManager* pWinMana = GetManaIntance();
//
//		switch (message)
//		{
//		case WM_DESTROY:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (!win) break;
//
//			if (pWinMana->winGL_IsMain(win->m_pGLWin, win))
//			{
//				win->GetGLWindow()->bClosed = true;
//				//PostQuitMessage(0);
//			}
//			else
//			{
//				pWinMana->Remove(win->m_hWnd);
//				win->OnDestroy();
//			}
//			break;
//		}
//		case WM_KEYDOWN:
//		case WM_SYSKEYDOWN:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetKeyboardStatus((int)wParam, true);
//			win->OnKeyBoard();
//			break;
//		}
//		case WM_KEYUP:
//		case WM_SYSKEYUP:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetKeyboardStatus((int)wParam, false);
//			win->OnKeyBoard();
//			break;
//		}
//		// Button event
//		case WM_LBUTTONUP:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetMouseButtonStatus(VK_LBUTTON, false);
//			win->OnMouseButton();
//			break;
//		}
//		case WM_RBUTTONUP:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetMouseButtonStatus(VK_RBUTTON, false);
//			win->OnMouseButton();
//			break;
//		}
//		case WM_LBUTTONDOWN:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetMouseButtonStatus(VK_LBUTTON, true);
//			win->OnMouseButton();
//			break;
//		}
//		case WM_RBUTTONDOWN:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->SetMouseButtonStatus(VK_RBUTTON, true);
//			win->OnMouseButton();
//			break;
//		}
//		case WM_MOUSEMOVE:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			win->OnMouseMove();
//			break;
//		}
//		case WM_SIZE: //Check if the window has been resized
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (!win) break;
//
//			win->m_width = LOWORD(lParam); // width
//			win->m_height = HIWORD(lParam); // height
//
//			glViewport(0, 0, win->m_width, win->m_height);
//			win->OnResize();
//			win->UpdateTitle();
//			win->UpdateTextRender();
//			win->OnDraw();
//			win->SwapBuffer();
//
//			return 0;
//		}
//		case WM_SIZING:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (!win) break;
//			glViewport(0, 0, win->m_width, win->m_height);
//			win->UpdateTitle();
//			return TRUE;
//		}
//		case WM_MOUSEWHEEL:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (win)
//			{
//				win->m_zDeltaScroll = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
//				win->OnMouseScroll();
//			}
//			break;
//		}
//		case WM_COMMAND:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (win)
//			{
//				WORD wID = LOWORD(wParam); // item, control, or accelerator identifier
//				WORD wEvt = HIWORD(wParam); // item event
//				HWND hwnd = (HWND)lParam;  // handle of control
//				win->OnCommandControl(hwnd, wID, wEvt);
//			}
//			break;
//		}
//		case WM_DRAWITEM:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (win)
//			{
//				WORD wID = LOWORD(wParam); // item, control, or accelerator identifier
//				HWND hwnd = (HWND)lParam;  // handle of control
//				LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
//				win->OnDrawControl(hwnd, wID);
//			}
//			break;
//		}
//		case WM_PAINT:
//		{
//			Window* win = pWinMana->Find(hWnd);
//			if (win)
//			{
//				win->OnPaint();
//			}
//			break;  //[BUG] always drawing
//		}
//		case WM_ERASEBKGND:
//		{
//			return TRUE;
//		}
//		default:
//		{
//			break;
//		}
//		}
//		return DefWindowProc(hWnd, message, wParam, lParam);
//	}
//
//public:
//	void AddWindow(Window* win)
//	{
//		// Init openGL extention use glew
//		static bool bInitOpenGLEx = false;
//		if (!bInitOpenGLEx)
//		{
//			bInitOpenGLEx = LoadOpenGLExtension();
//		}
//
//		if (win->_OnCreate(GL_WIN_CLASS, bInitOpenGLEx))
//		{
//			WindowManager* pWinMana = GetManaIntance();
//
//			win->m_pGLWin = this;
//			if (pWinMana->Add(win))
//			{
//				win->Show();
//			}
//			else
//			{
//				win->OnDestroy();
//			}
//		}
//	}
//
//	Window* GetMainWin()
//	{
//		WindowManager* pWinMana = GetManaIntance();
//		return pWinMana->winGL_GetFront(this);
//	}
//
//	GLWindow()
//	{
//		RegisterWindowClass(GL_WIN_CLASS, WndMainProc, GetModuleHandle(NULL));
//	}
//
//	~GLWindow()
//	{
//		WindowManager* pWinMana = GetManaIntance();
//		auto winlist = pWinMana->winGL_GetListWindow(this);
//
//		for (int i = 0; i < winlist.size(); i++)
//		{
//			pWinMana->Remove(winlist[i]->m_hWnd);
//			winlist[i]->OnDestroy();
//		}
//	}
//
//public:
//
//	void Process()
//	{
//		WindowManager* pWinMana = GetManaIntance();
//		auto winlist = pWinMana->winGL_GetListWindow(this);
//
//		for (int i = 0; i < winlist.size(); i++) {
//			winlist[i]->UpdateTime();
//			winlist[i]->OnProcess();
//		}
//	}
//
//	void Draw()
//	{
//		WindowManager* pWinMana = GetManaIntance();
//		auto winlist = pWinMana->winGL_GetListWindow(this);
//
//		for (int i = 0; i < winlist.size(); i++) {
//			if (winlist[i]->MakeContext())
//			{
//				winlist[i]->OnDraw();
//				winlist[i]->SwapBuffer();
//			}
//		}
//	}
//
//	bool Closed()
//	{
//		return !bClosed;
//	}
//
//	void PollEvent()
//	{
//		// Xử lý trong trường hợp vẽ liên tục
//		// Check to see if any messages are waiting in the queue
//		while (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
//		{
//			// Translate the message and dispatch it to WindowProc()
//			TranslateMessage(&m_msg);
//			DispatchMessage(&m_msg);
//		}
//	}
//
//	void WaitEvent()
//	{
//		// Xử lý trong trường hợp vẽ theo sự kiện
//		if (GetMessage(&m_msg, NULL, 0, 0) > 0)
//		{
//			TranslateMessage(&m_msg);
//			DispatchMessage(&m_msg);
//		}
//	}
//};
//

____END_NAMESPACE____

#endif // WGLHANDLE_H