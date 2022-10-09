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

void Draw(Window* win)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	{
		glVertex2f(0.5, 0.5);
		glVertex2f(-0.5, 0.5);
	}
	glEnd();
}

int main()
{
	Window* win = fox_create_window(L"thường", 0, 0);
	win->SetOnDrawfunc(Draw);
	win->WriteSystemInfo(true);

	if (win)
	{
		while (!win->closed())
		{
			win->draw();
			win->poll_event();
		}
	}
	fox_destroy_window(win);

}
