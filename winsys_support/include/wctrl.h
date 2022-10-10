#pragma once
////////////////////////////////////////////////////////////////////////////////////
// File: WinControl.cpp   -- Create and handle event control the window             
// Copyright (C) 2020-2022 Thuong.NV   Created : 22/04/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////

#include <CommCtrl.h>
#include <WinUser.h>
#include <windows.h>
#include <vector>

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
	Control(CtrlType type)
	{
		m_ID = 0;
		m_type = type;
		m_hwnd = NULL;
	}

public:
	virtual void     OnInitControl(UINT& IDS) = 0;
	virtual CtrlType GetType() = 0;

	virtual void     OnDestroy() {};
	virtual void     Draw() {};
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
		ShowWindow(m_hwnd, m_bVisble ? TRUE : FALSE);
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

	virtual void OnInitControl(UINT& IDS)
	{
		if (CreateMenuContext(IDS))
		{

		}
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

	virtual void OnInitControl(UINT& IDS)
	{
		if (CreateMenuBar(IDS))
		{

		}
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
	enum { HEIGHT_DEF = 20 };

protected:
	int         m_x;
	int         m_y;
	UINT        m_width;
	UINT        m_height;

	wstring      m_label;
	bool        m_bClicked;

	void(*m_EventFun)(Window* window, Button* btn) = NULL;
public:
	Button() : Control(CtrlType::BUTTON),
		m_width(WIDTH_DEF), m_height(HEIGHT_DEF)
	{
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
	void OnInitControl(UINT& IDS)
	{
		UINT BackupIDS = IDS;
		m_ID = IDS++;
		m_hwnd = (HWND)CreateWindow(L"BUTTON", m_label.c_str(),     // Button text 
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
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
		}
		this->UpdateFontFromParant();
	}

	virtual void Event(Window* window, WORD _id, WORD _event)
	{
		m_bClicked = true;
		if (!m_EventFun) return;

		m_EventFun(window, this);
	}

	void SetEvent(void(*mn)(Window*, Button*))
	{
		m_EventFun = mn;
	}

	void Draw()
	{
		UpdateWindow(m_hwnd);
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
	void OnInitControl(UINT& IDS)
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
		}

		UpdateItems();
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