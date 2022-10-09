#include <iostream>
#include "include/sysutils.h"
#include "include/dbstruct.h"
#include <thread>
#include "include/sysreport.h"
#include "include/wglhandle.h"

___USELIBFOX____;

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

void MouseButton(Window* win)
{
	if (win->GetMouseButtonStatus(GLMouse::LeftButton))
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

void Process(Window* win)
{
	//for (int i = 0; i < 12; i++)
	//{
	//	for (int j = 0; j < 100; j++)
	//	{
	//		x += 0.001;
	//		std::this_thread::sleep_for(10ms);
	//	}
	//}
}


void Draw(Window* win)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex2f(-x, a);
		glVertex2f( x, a);
	}
	glEnd();
	//a += 0.5* win->GetFrameTime();
}

int main()
{
	Window* win = fox_create_window(L"thường", 0, 0);
	win->SetOnDrawfunc(Draw);
	win->SetProcessfunc(Process);
	win->SetOnMouseButtonfunc(MouseButton);

	win->WriteSystemInfo(true);

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

}
