// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__3C180D69_7355_11D3_9CCB_00400543CD04__INCLUDED_)
#define AFX_STDAFX_H__3C180D69_7355_11D3_9CCB_00400543CD04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

// You can use 0x0400 here also, if you need to support older systems.
#ifndef WINVER
#	define WINVER         0x0500
#	define _WIN32_WINNT   0x0500
#	define _WIN32_WINDOWS 0x0500
#	define _WIN32_IE      0x0500
#endif

#define _CRT_SECURE_NO_WARNINGS 1

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3C180D69_7355_11D3_9CCB_00400543CD04__INCLUDED_)
