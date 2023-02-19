#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <Windows.h>
#include <memory>
#include <fstream>
#include "xsysdef.h"

___BEGIN_NAMESPACE___

/******************************************************************************
*! @brief  : tracker function support
*! @author : thuong.nv - [Date] : 31/10/2022
*! @return : void
******************************************************************************/
std::string tracker_to_utf8(const wchar_t* mb, const int& nsize)
{
    std::string utf8;
    utf8.resize(nsize +sizeof(wchar_t), 0);
    int nbytes = WideCharToMultiByte(CP_UTF8, 0, mb, (int)nsize/sizeof(wchar_t),
        (LPSTR)utf8.c_str(), (int)utf8.size(), NULL, NULL);
    utf8.resize(nbytes);
    return utf8;
}

/******************************************************************************
*! @brief  : tracker get date time now in system local
*! @author : thuong.nv - [Date] : 31/10/2022
*! @return : void
******************************************************************************/
void tracker_get_datetime(wchar_t* buff)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	const size_t buffsize = 200;
	wchar_t bufftime[buffsize];

	memset(bufftime, 0, buffsize);
	swprintf_s(bufftime, L"<%04d%02d%02d %02d:%02d:%02d:%03d>",
		SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay,
		SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond, SystemTime.wMilliseconds);

	memcpy_s(buff, wcslen(bufftime) * sizeof(wchar_t), bufftime, wcslen(bufftime) * sizeof(wchar_t));
}

/******************************************************************************
*! @brief  : Write msg to file tracker.log
*! @author : thuong.nv - [Date] : 31/10/2022
*! @return : void
******************************************************************************/
void _tracker_save(const wchar_t* format, va_list args)
{
	static std::wstring filepath = L"";
	if (filepath.empty())
	{
		wchar_t path[MAX_PATH];
		if (0 >= ::GetModuleFileName(NULL, path, sizeof(path)))
			return;

		std::wstring module_path = path;
		std::wstring file_name = L"Tracker\\";

		module_path = module_path.substr(0, module_path.rfind('\\') + 1);
		filepath = module_path + file_name;

		if (CreateDirectory(filepath.c_str(), NULL) || // create ok
			ERROR_ALREADY_EXISTS == GetLastError())
		{
			filepath.append(L"trace.log");
		}
		else
		{
			filepath = L"";
			return;
		}
	}

	if (filepath.empty())
		return;

	// write tracker to file
	FILE* file = _wfsopen(filepath.c_str(), L"a+", SH_DENYNO);
	if (!file)
		return;

	const size_t nmaxmsg = 2048; // max buffer write

	wchar_t msg[nmaxmsg];
	int nprint = vswprintf_s(msg, nmaxmsg, format, args);

	std::string temp = tracker_to_utf8(msg, nprint * sizeof(wchar_t));
	fputs(temp.c_str(), file);
	fputs("\n", file);

	fclose(file);
}

/******************************************************************************
*! @brief  : write log use format and argument list
*! @author : thuong.nv - [Date] : 31/10/2022
*! @return : void
******************************************************************************/
void tracker_save(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	_tracker_save(format, args);
	va_end(args);
}

enum tracker_state
{
	eTracker_None    = 0x0000,
	eTracker_Error   = 0x0001,
	eTracker_Warning = 0x0002,
	eTracker_Assert  = 0x0004,
};

void tracker_outstring(const char* filename, int linenum, bool savetime, const wchar_t* format, ...)
{
	const size_t nmaxmsg = 2048;
	const size_t ndatetime = 200;
	const size_t nmaxfilename = 200;

	wchar_t msg[nmaxmsg];
	memset(msg, 0, nmaxmsg);

	wchar_t fname[nmaxfilename];
	memset(fname, 0, nmaxfilename);

	size_t outSize;
	mbstowcs_s(&outSize, &fname[0], nmaxfilename, filename, strlen(filename));

	if (savetime)
	{
		wchar_t datetime[200]; memset(datetime, 0, ndatetime);
		tracker_get_datetime(datetime);
		swprintf_s(msg, nmaxmsg, L"%s %s [%s %d]", datetime, format, fname, linenum);
	}
	else
	{
		swprintf_s(msg, nmaxmsg, L"%s [%s %d]", format, fname, linenum);
	}

	va_list args;
	va_start(args, format);
	_tracker_save(msg, args);
	va_end(args);
}

void tracker_outstring_state(int state, const char* filename, int linenum, bool savetime, const wchar_t* format, ...)
{
	wchar_t fstate[50];
	memset(fstate, 0, 50);
	
	if (state & eTracker_Warning)
	{
		memcpy(fstate, L"[__WARNING__]", sizeof(L"[__WARNING__]"));
	}
	else if (state & eTracker_Error)
	{
		memcpy(fstate, L"[___ERROR___]", sizeof(L"[___ERROR___]"));
	}

	else if (state & eTracker_Assert)
	{
		memcpy(fstate, L"[___ASSERT__]", sizeof(L"[___ASSERT__]"));
	}

	const size_t nmaxmsg = 2048;
	const size_t ndatetime = 200;
	const size_t nmaxfilename = 200;

	wchar_t msg[nmaxmsg];
	memset(msg, 0, nmaxmsg);

	wchar_t fname[nmaxfilename];
	memset(fname, 0, nmaxfilename);

	size_t outSize;
	mbstowcs_s(&outSize, &fname[0], nmaxfilename, filename, strlen(filename));

	if (savetime)
	{
		wchar_t datetime[200]; memset(datetime, 0, ndatetime);
		tracker_get_datetime(datetime);
		swprintf_s(msg, nmaxmsg, L"%s %s %s [%s %d]", datetime, fstate, format, fname, linenum);
	}
	else
	{
		swprintf_s(msg, nmaxmsg, L"%s %s [%s %d]", fstate, format, fname, linenum);
	}

	va_list args;
	va_start(args, format);
	_tracker_save(msg, args);
	va_end(args);
}


#ifdef _DEBUG
#define FOX_TRACE(fm, ...)			tracker_outstring(__FUNCTION__,__LINE__,true, fm, __VA_ARGS__)
#define FOX_TRACE_ERROR(fm, ...)	tracker_outstring_state(eTracker_Error, __FUNCTION__,__LINE__,true, fm, __VA_ARGS__)
#define FOX_TRACE_WARNING(fm, ...)	tracker_outstring_state(eTracker_Warning, __FUNCTION__,__LINE__,true, fm, __VA_ARGS__)
#define FOX_TRACE_ASSERT(fm, ...)	tracker_outstring_state(eTracker_Assert , __FUNCTION__,__LINE__,true, fm, __VA_ARGS__)
#else
#define FOX_TRACE(fm, ...)			
#define FOX_TRACE_ERROR(fm, ...)	
#define FOX_TRACE_WARNING(fm, ...)	
#define FOX_TRACE_ASSERT(fm, ...)	
#endif // DEBUG


____END_NAMESPACE____