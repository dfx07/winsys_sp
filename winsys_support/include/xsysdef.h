// File: xsysdef.h   -- define system                                               
// Copyright (C) 2021-2022 Thuong.NV   Created : 08/10/2022                         
// For conditions of distribution and use, see copyright notice in readme.txt       
////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <gdiplus.h>

#define IN  /*INPUT */
#define OUT /*OUTPUT*/

#define ___NAMESPACE_BEGIN___ namespace fox {
#define ___NAMESPACE_END___					}

#define	___USELIBFOX____	  using namespace fox



// Information monitor
struct MonitorInfo
{
	DWORD		  WIDTH;
	DWORD		  HEIGHT;
	DWORD		  DISFREQ;
	DWORD		  VERSION;
	std::string   NAME;
};

struct GDIplusToken
{
	ULONG_PTR						 m_id;
	Gdiplus::GdiplusStartupInput     m_prop;
};

