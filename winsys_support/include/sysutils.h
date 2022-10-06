#pragma once
#include <iostream>
#include <memory>
#include <Windows.h>
#include <stdint.h>

#include "systruct.h"

#define IN  /*INPUT */
#define OUT /*OUTPUT*/

/******************************************************************************
*! @brief  : Read bytes data file
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : int : nsize / data : buff
******************************************************************************/
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
CFileBuffer* read_data_file(IN const wchar_t* path)
{
    int nbytes = 0;
    // open and read file share data
    FILE* file = _wfsopen(path, L"rb", _SH_DENYRD);
    if (!file) return NULL;

    // read number of bytes file size 
    fseek(file, 0L, SEEK_END);
    nbytes = static_cast<int>(ftell(file));
    fseek(file, 0L, SEEK_SET);

    // read data form file to memory
    CFileBuffer* cfbuff = new CFileBuffer(nbytes + 2);
    nbytes = (int)fread_s(cfbuff->get(), nbytes, sizeof(char), nbytes, file);
    cfbuff->resize(nbytes);

    fclose(file);
    // Read content bytes file + nbyte read
    return cfbuff;
}

/******************************************************************************
*! @brief  : Write data to file binary
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : true | false
******************************************************************************/
bool write_data_file(IN const wchar_t* path, IN const void* data, IN const int& nsize)
{
    FILE* file = _wfsopen(path, L"wb", SH_DENYNO);
    if (!file) return false;

    fwrite(data, sizeof(char), nsize, file);

    fclose(file);
    return true;
}

/******************************************************************************
*! @brief  : Create list folder follow path
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : true : exist / false: not exist
******************************************************************************/
bool create_directory_recursive(IN const std::wstring& path)
{
    BOOL bret = CreateDirectory(path.c_str(), NULL);

    if (bret)  return true;
    else
    {
        DWORD dwErr = GetLastError();
        if (dwErr == ERROR_ALREADY_EXISTS)
            return true;
        if ((dwErr == ERROR_PATH_NOT_FOUND || dwErr == ERROR_FILE_NOT_FOUND))
        {
            std::wstring subpath = path.substr(0, path.find_last_of('\\'));

            if (create_directory_recursive(subpath))
            {
                return CreateDirectory(path.c_str(), NULL)? true : false;
            }
        }
    }
    return false;
}

/******************************************************************************
*! @brief  : Convert char to wchar_t
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : bytes array
******************************************************************************/
std::wstring from_utf8(IN const std::string& utf8)
{
    std::wstring utf16;
    utf16.resize(utf8.size() + 1, 0);
    int nWide = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8.c_str(),
                   (int)utf8.length(), (LPWSTR)utf16.c_str(), (int)utf16.size());
    utf16.resize(nWide);
    return utf16;
}
// nbyte
std::wstring from_utf8(IN const char* utf8, IN const int& nsize)
{
    std::wstring utf16;
    utf16.resize(nsize + 1, 0);
    int nWide = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, utf8,
        nsize, (LPWSTR)utf16.c_str(), (int)utf16.size());
    utf16.resize(nWide);
    return utf16;
}

/******************************************************************************
*! @brief  : Convert char to wchar_t
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : bytes array
******************************************************************************/
std::string to_utf8(IN const std::wstring& mb)
{
    std::string utf8;
    utf8.resize(mb.size()*sizeof(WCHAR) + 1, 0);
    int nbytes = WideCharToMultiByte(CP_UTF8, 0, mb.c_str(), (int)mb.length(),
        (LPSTR)utf8.c_str(), (int)utf8.size(), NULL, NULL);
    utf8.resize(nbytes);
    return utf8;
}

/******************************************************************************
*! @brief  : Check array bytes is utf8
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : 1 : utf8 | 0 : no utf8
*! @note   : https://unicodebook.readthedocs.io/guess_encoding.html
******************************************************************************/
int is_utf8(const char* data, size_t size)
{
    const unsigned char* str = (unsigned char*)data;
    const unsigned char* end = str + size;
    unsigned char byte;
    unsigned int code_length, i;
    uint32_t ch;
    while (str != end) {
        byte = *str;
        if (byte <= 0x7F) {
            /* 1 byte sequence: U+0000..U+007F */
            str += 1;
            continue;
        }

        if (0xC2 <= byte && byte <= 0xDF)
            /* 0b110xxxxx: 2 bytes sequence */
            code_length = 2;
        else if (0xE0 <= byte && byte <= 0xEF)
            /* 0b1110xxxx: 3 bytes sequence */
            code_length = 3;
        else if (0xF0 <= byte && byte <= 0xF4)
            /* 0b11110xxx: 4 bytes sequence */
            code_length = 4;
        else {
            /* invalid first byte of a multibyte character */
            return 0;
        }

        if (str + (code_length - 1) >= end) {
            /* truncated string or invalid byte sequence */
            return 0;
        }

        /* Check continuation bytes: bit 7 should be set, bit 6 should be
         * unset (b10xxxxxx). */
        for (i = 1; i < code_length; i++) {
            if ((str[i] & 0xC0) != 0x80)
                return 0;
        }

        if (code_length == 2) {
            /* 2 bytes sequence: U+0080..U+07FF */
            ch = ((str[0] & 0x1f) << 6) + (str[1] & 0x3f);
            /* str[0] >= 0xC2, so ch >= 0x0080.
               str[0] <= 0xDF, (str[1] & 0x3f) <= 0x3f, so ch <= 0x07ff */
        }
        else if (code_length == 3) {
            /* 3 bytes sequence: U+0800..U+FFFF */
            ch = ((str[0] & 0x0f) << 12) + ((str[1] & 0x3f) << 6) +
                (str[2] & 0x3f);
            /* (0xff & 0x0f) << 12 | (0xff & 0x3f) << 6 | (0xff & 0x3f) = 0xffff,
               so ch <= 0xffff */
            if (ch < 0x0800)
                return 0;

            /* surrogates (U+D800-U+DFFF) are invalid in UTF-8:
               test if (0xD800 <= ch && ch <= 0xDFFF) */
            if ((ch >> 11) == 0x1b)
                return 0;
        }
        else if (code_length == 4) {
            /* 4 bytes sequence: U+10000..U+10FFFF */
            ch = ((str[0] & 0x07) << 18) + ((str[1] & 0x3f) << 12) +
                ((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
            if ((ch < 0x10000) || (0x10FFFF < ch))
                return 0;
        }
        str += code_length;
    }
    return 1;
}

/******************************************************************************
*! @brief  : Check array bytes is ascii character
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : 1 : ascii | 0 : no ascii
*! @note   : https://unicodebook.readthedocs.io/guess_encoding.html
******************************************************************************/
int is_ascii(const char* data, size_t size)
{
    const unsigned char* str = (const unsigned char*)data;
    const unsigned char* end = str + size;
    for (; str != end; str++) {
        if (*str & 0x80)
            return 0;
    }
    return 1;
}

/***************************************************************************
*! @brief  : Get encoding use bom bytes
*! @author : thuong.nv - [Date] : 03/10/2022
*! @return : ANSI=0 | UTF-8=1 | UTF-16LE=2 | UTF-16BE=3 | UTF-32LE=4 | UTF-32BE=5
*! @note   : N/A
***************************************************************************/
int encoding_bytes_bom(unsigned char* _bytes, const int& _size)
{
    if (!_bytes || !_size) return 0;

    unsigned char utf8[]    = { 0xEF, 0xBB, 0xBF };
    unsigned char utf16le[] = { 0xFF, 0xFE };
    unsigned char utf16be[] = { 0xFE, 0xFF };
    unsigned char utf32le[] = { 0xFF, 0xFE, 0x00, 0x00 };
    unsigned char utf32be[] = { 0x00, 0x00, 0xFE, 0xFF };

    if (_size >= 3)
    {
        // utf-8
        if (_bytes[0] == utf8[0] && _bytes[1] == utf8[1] &&
            _bytes[2] == utf8[2])
            return 1;
    }
    if (_size >= 4)
    {
        // utf-32le
        if (_bytes[0] == utf32le[0] && _bytes[1] == utf32le[1] &&
            _bytes[2] == utf32le[2] && _bytes[3] == utf32le[3])
            return 4;

        // utf-32be
        if (_bytes[0] == utf32be[0] && _bytes[1] == utf32be[1] &&
            _bytes[2] == utf32be[2] && _bytes[3] == utf32be[3])
            return 5;
    }
    if (_size >= 2)
    {
        // utf-16le
        if (_bytes[0] == utf16le[0] && _bytes[1] == utf16le[1])
            return 2;

        // utf-16be
        if (_bytes[0] == utf16be[0] && _bytes[1] == utf16be[1])
            return 3;
    }

    return 0; // ansi
}

/***************************************************************************
*! @brief  : Check system endian is using
*! @author : thuong.nv - [Date] : 03/10/2022
*! @param  : void
*! @return : 0 =little endian , 1 =big endian
***************************************************************************/
int system_endian()
{
    // little endian if true
    int n = 1;
    return (*(char*)&n == 1) ? 0 : 1;
}

/***************************************************************************
*! @brief  : reverse 16 bit 8H<->8L
*! @author : thuong.nv - [Date] : 03/10/2022
*! @param  : wchar_t character
*! @return : 0 :little endian , 1 :big endian
*! @note   : N/A
***************************************************************************/
void reverse_byte_16(wchar_t* ch)
{
    *ch = ((*ch << 8) | (*ch >> 8));
}
wchar_t reverse_byte_16x(wchar_t* ch)
{
    return (*ch >> 8) | (*ch << 8);
}