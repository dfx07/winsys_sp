#pragma once
#include <iostream>
#include <memory>

#define IN 
#define OUT

typedef std::shared_ptr<void*> BUFF_DATA_Ptr;
// @brief  read bytes data file
// @return int : nsize && data : buff
int read_data_file(IN const wchar_t* path, OUT void** data)
{
    if (data) *data = NULL;
    int nbytes = 0;
    // open and read file share data
    FILE* file = _wfsopen(path, L"rb", _SH_DENYRD);
    if (!file) return nbytes;

    // read number of bytes file size 
    fseek(file, 0L, SEEK_END);
    nbytes = static_cast<int>(ftell(file));
    fseek(file, 0L, SEEK_SET);

    // read data form file to memory
    auto tbuff = new unsigned char[nbytes + 2];
    memset(tbuff, 0, (nbytes + 2));
    nbytes = (int)fread_s(tbuff, nbytes, sizeof(char), nbytes, file);
    fclose(file);

    // Read content bytes file + nbyte read
    if (data) *data = tbuff;
    else delete[] tbuff;
    return nbytes;
}

// @brief write data to file binary
// @return true | false
bool write_data_file(const wchar_t* path, const void* data, const int& nsize)
{
    FILE* file = _wfsopen(path, L"wb", SH_DENYNO);
    if (!file) return false;

    fwrite(data, sizeof(char), nsize, file);

    fclose(file);
    return true;
}
