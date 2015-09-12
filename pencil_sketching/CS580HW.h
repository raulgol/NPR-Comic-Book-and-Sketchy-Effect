// CS580HW.h : main header file for the CS580HW application
//

#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCS580HWApp:
// See CS580HW.cpp for the implementation of this class
//

class CCS580HWApp : public CWinApp
{
public:
	CCS580HWApp();

	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};