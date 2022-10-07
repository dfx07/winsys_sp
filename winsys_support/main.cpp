#include <iostream>
//#include "include/systruct.h"
#include "include/sysutils.h"
#include "include/dbstruct.h"
#include <thread>
#include "include/sysreport.h"

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
class A
{
public:
    int a;
};

int main()
{
    //std::thread t(process_time);
    //std::thread t1(process_output);
    //t.join();
    //t1.join();

    //SetUnhandledExceptionFilter(handle_crash);

    //handle_crash(NULL);

    //A* a = NULL;
    //int c = a->a;

    //bool b = is_directory_exist(std::wstring( L""));


    auto b = get_folder_path(std::wstring(L"C:\\Users\\Mark\\Desktop1\\delta.cpp"));



    int a = 100;
    int c = a *2;
    getchar();
}
