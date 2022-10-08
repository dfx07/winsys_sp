#include <iostream>
#include "include/sysutils.h"
#include "include/dbstruct.h"
#include <thread>
#include "include/sysreport.h"

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
class A
{
public:
    int a;
};

int main()
{
	fox::CFPSCouter counter;
	counter.start();
	while (true)
	{
		counter.update();
		std::cout << counter.elapsed() << std::endl;
		for (int i = 0; i < 1000000; i++)
		{
			//for (int j = 0; j < 1000; j++)
			//{
			//	int c = 10;
			//}
		}
	}
    getchar();
}
