// ODBC Binding.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CODBCBindingApp:
// See ODBC Binding.cpp for the implementation of this class
//

class CODBCBindingApp : public CWinApp
{
public:
	CODBCBindingApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CODBCBindingApp theApp;

inline BOOL MySQLSuccess(SQLRETURN rc) {
	return (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO);
}