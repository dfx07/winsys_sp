////////////////////////////////////////////////////////////////////////////////////
/*!*********************************************************************************
* @Copyright (C) 2021-2022 thuong.nv <thuong.nv.mta@gmail.com>
*            All rights reserved.
************************************************************************************
* @file     xsysdef.h
* @create   Nov 15, 2022
* @brief    Library define system on window os
* @note     For conditions of distribution and use, see copyright notice in readme.txt
************************************************************************************/
#ifndef XSYSDEF_H
#define XSYSDEF_H

#undef  IN
#define IN  /*INPUT */

#undef  OUT
#define OUT /*OUTPUT*/

#undef  interface
#define interface struct /*interface*/

#define ___BEGIN_NAMESPACE___ namespace fox	  {
#define ____END_NAMESPACE____				  }
#define	____USE_NAMESPACE____ using namespace fox;


#ifdef _EXPORT_
	#define Dllexport  __declspec( dllexport )
#else
	#define Dllexport
#endif

#define SAFE_DELETE(p) {delete p; p = NULL;}
#define SAFE_DELETE_ARRAY(p) {delete[] p; p = NULL;}

#endif // XSYSDEF_H
