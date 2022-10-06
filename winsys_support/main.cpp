#include <iostream>
//#include "include/systruct.h"
#include "include/sysutils.h"
#include "include/dbstruct.h"
#include <thread>

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

int main()
{
    std::thread t(process_time);
    std::thread t1(process_output);
	std::cout << CTimer::time_now("%X").c_str() << std::endl;
    t.join();
    t1.join();

    getchar();
}
