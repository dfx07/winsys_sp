﻿#include "include/xsysutils.h"
#include "include/dbstruct.h"
#include <thread>
#include "include/wglhandle.h"
#include "include/xsystrace.h"
#include <ppl.h>

____USE_NAMESPACE____

class Object
{
    int m_data = 100;
public:
    void Update()
    {
        m_data++;

        if (m_data > 20)
        {
            m_data = 0;
        }
    }

    int getdata()
    {
        return m_data;
    }
};

Object obj;

void process_time()
{
    CStopwatch sw;
    for (;;)
    {

        obj.Update();
    }
}

void process_output()
{
    CTimer timer;
    timer.reset();
    while (true)
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        //int a = obj.getdata();
        //std::cout << timer.mili_elapsed() << std::endl;
    }
}

float x = 0.5;
float a = -1.0;

MenuContext* menu = NULL;

void MouseButton(Window* win, int btn, int action)
{
    if (action == GL_PRESSED)
    {
        if (btn == GLMouse::RightButton)
        {
            menu->Show(win->GetCursorPos());
        }
    }
    //std::cout << a << std::endl;
    if (action == GL_PRESSED)
    {
        if (btn == GLMouse::LeftButton)
        {
            if (a == -1.0)
            {
                a = 1.0;
            }
            else
            {
                a = -1.0;
            }
        }
    }
}

void Process(Window* win)
{
    //for (int i = 0; i < 100000000; i++)
    //{
    //	for (int j = 0; j < 100000000; j++)
    //	{
    //		//x += 0.001;
    //		int a = 10;
    //		//std::this_thread::sleep_for(10ms);
    //	}
    //}
}


void Draw(Window* win)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor( 120.0 / 255.0, 139.f / 255.0, 201.0 / 255.0, 1.0);

    
    //glColor3f(1.0, 0.0, 0.0);
    //glBegin(GL_LINES);
    //{
    //	glVertex2f(-x, a);
    //	glVertex2f( x, a);
    //}
    //glEnd();
    //a += 0.5* win->GetFrameTime();
}

void btnenter(Window* win, Button* btn)
{
	btn->SetLabel(L">> 조선말 <<");
}

void btnleave(Window* win, Button* btn)
{
	btn->SetLabel(L"조선말");
	std::cout << "leave" << std::endl;
}

int main()
{
    WndProp adven;
    adven.m_iAntialiasing = 8;
    //adven.m_bFullScreen = true;
    adven.m_iModeDraw = 0;
    adven.m_bGDIplus = false;

    MenuItemBase item;
    item.SetLabel(L"Open file");

    menu = new MenuContext();
    menu->AddItem(item);


    //Label* label = new Label();

    //label->SetText(L"Nguyễn Thị Hoài Thi");
    //label->SetPosition(10, 300);
    //label->SetColor(100, 100, 100);

    Button* btn = new Button();
    btn->SetLabel(L"조선말");
    btn->SetPosition(10, 30);
    btn->SetSize(300, 100);
	btn->SetEventEnterCallBack(btnenter);
	btn->SetEventLeaveCallBack(btnleave);

    Button* btn1 = new Button();
    btn1->SetLabel(L"조선말");
    btn1->SetPosition(10, 30+100);
    btn1->SetSize(300, 100);

    Button* btn2 = new Button();
    btn2->SetLabel(L"조선말");
    btn2->SetPosition(10, 30+200);
    btn2->SetSize(300, 100);

    Button* btn3 = new Button();
    btn3->SetLabel(L"조선말");
    btn3->SetPosition(10, 30+300);
    btn3->SetSize(300, 100);

    //Combobox* cbb = new Combobox();
    //cbb->AddItem(L"조선말", new int(5));
    //cbb->SetPosition(10, 400);

    Window* win = fox_create_window(L"thường", 0, 0, 640, 480, &adven);

    win->SetOnDrawfunc(Draw);
    win->SetProcessfunc(Process);
    win->SetOnMouseButtonfunc(MouseButton);


    win->SetFont(L"Segoe UI", 14);

    if (win)
    {
        while (!win->closed())
        {
            win->process();
            win->draw();
            win->poll_event();
        }
    }
    fox_destroy_window(win);

    return 0;
}

void check()
{
}