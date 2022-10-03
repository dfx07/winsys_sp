#include <iostream>
//#include "include/systruct.h"
#include "include/sysutils.h"

int main()
{
    ; int nbytes;
    CFileBuffer* file = read_data_file(L"test.data");
    nbytes = file->size();

    std::wstring temp = from_utf8((char*)file->get());

    std::string utf8 = to_utf8(temp);
    getchar();
}
