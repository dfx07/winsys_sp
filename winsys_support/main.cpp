#include <iostream>
//#include "include/systruct.h"
#include "include/sysutils.h"
#include "include/dbstruct.h"

class A
{
public:
    int c = 10;
};

int function()
{
    auto row = CDataRow::CreateInstance();

    row->Add("column 4", L"thuong");

    CDataTable table;
    
    table.AddRow(row);


    auto obj = table.SingleOrDefault<wchar_t*>(nullptr);
   // obj->c = 100000;
    return 0;
}

int main()
{
    function();
    getchar();
}
