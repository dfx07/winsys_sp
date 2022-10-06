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
    for (;;)
    {
        for (int j = 0; j < 100000000; j++)
        {
            for (int jj = 0; jj < 100000000; jj++)
            {
                int c = 100;
            }
            int a = 10;
        }
        obj.Update();
    }
}

void process_output()
{
    CTimer timer;
    timer.reset();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        int a = obj.getdata();
        std::cout << timer.duration() << std::endl;
    }
}

int main()
{
    std::thread t(process_time);
    std::thread t1(process_output);
    t.join();
    t1.join();

    getchar();
}
