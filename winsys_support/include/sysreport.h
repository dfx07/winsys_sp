#pragma once

#include <iostream>
#include <Windows.h>
#include <Dbghelp.h>
#include <tchar.h>
#include "sysutils.h"

___NAMESPACE_BEGIN___

// define function create dump file
typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(
    HANDLE hProcess,
    DWORD dwPid, 
    HANDLE hFile,
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, 
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

// Setup use dump file : for Window 
// - setup application : Windpb preview
// - run program to crash ->*.dmp
// - analysis and export file *.txt
bool create_dump_file(const std::wstring& path, struct _EXCEPTION_POINTERS* apExceptionInfo)
{
    HMODULE mhLib = ::LoadLibrary(_T("dbghelp.dll"));
    if (!mhLib)
        return false;
    auto pDump = (MINIDUMPWRITEDUMP)(::GetProcAddress(mhLib, "MiniDumpWriteDump"));

    HANDLE  hFile = ::CreateFile(path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile && hFile != INVALID_HANDLE_VALUE)
    {
        _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
        ExInfo.ThreadId = ::GetCurrentThreadId();
        ExInfo.ExceptionPointers = apExceptionInfo;
        ExInfo.ClientPointers = FALSE;

        MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory);

        pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, &ExInfo, NULL, NULL);
        ::CloseHandle(hFile);

		return true;
    }
    return false;
}

// handle crash program
// Please use if hanlde crash: SetUnhandledExceptionFilter(handle_crash);
LONG WINAPI handle_crash(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
    TCHAR	szAppFullPath[MAX_PATH] = L"";
    ::GetModuleFileName(NULL, szAppFullPath, MAX_PATH);

    std::wstring dumpfileFolder = get_folder_path<std::wstring>(szAppFullPath);
    std::wstring dumpfileName = get_filename_path<std::wstring>(szAppFullPath);
	MessageBox(NULL, L"The program terminates abnormally, please restart !", L"Crash", MB_ICONHAND | MB_OK);

	if (dumpfileName.empty() || dumpfileFolder.empty())
        return EXCEPTION_CONTINUE_EXECUTION;
	dumpfileFolder.append(L"DumpLoger\\");

	if (!create_directory_recursive(dumpfileFolder))
		return EXCEPTION_CONTINUE_EXECUTION;

    create_dump_file(dumpfileFolder.append(dumpfileName.append(L".dmp")), apExceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}

___NAMESPACE_END___