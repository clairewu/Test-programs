// ODBC BindingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ODBC Binding.h"
#include "ODBC BindingDlg.h"
#include ".\odbc bindingdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const TCHAR *TARGET_TABLE_NAME = _T("Contact");

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CODBCBindingDlg dialog



CODBCBindingDlg::CODBCBindingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CODBCBindingDlg::IDD, pParent)
	, m_strPassword(_T("ORA31150"))
	, m_strUsername(_T("ORA31150"))
	, m_strContent(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CODBCBindingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DSN, m_cbDSN);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strUsername);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_strPassword);
	DDX_Text(pDX, IDC_EDIT_CONTENT, m_strContent);
}

BEGIN_MESSAGE_MAP(CODBCBindingDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INSERT_TO_TABLE, OnBnClickedButtonInsertToTable)
END_MESSAGE_MAP()


// CODBCBindingDlg message handlers

BOOL CODBCBindingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LoadSysDSNs();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CODBCBindingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CODBCBindingDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CODBCBindingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CODBCBindingDlg::LoadSysDSNs(void)
{
	HKEY hKey;

	const TCHAR szSubKey[] = _T("Software\\ODBC\\ODBC.INI\\ODBC Data Sources\\");

	LONG ret = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_ALL_ACCESS, &hKey);

	CString strMsg;
	strMsg.LoadString(IDS_ERROR_LOADING_DSN);
	if(ERROR_SUCCESS != ret)
	{
		MessageBox(strMsg);
		PostQuitMessage(0);

		return;
	}

	TCHAR	szName[MAX_PATH];
	BYTE 	szData[256];
	DWORD 	dwNameLen, dwDataLen(MAX_PATH), dwType, dwIndex(0);

	do
	{
		dwNameLen = MAX_PATH;
		ret = ::RegEnumValue(hKey, dwIndex++, szName, &dwNameLen, NULL, NULL, NULL, NULL);
		
		if(ERROR_SUCCESS == ret)
			m_cbDSN.AddString(szName);
	} while(ERROR_SUCCESS == ret);

	if(m_cbDSN.GetCount() == 0)
	{
		MessageBox(_T("No DSN discovered! Please add DSN before running this program!\nProgram will now exit!"), _T("Error"), MB_ICONERROR);
		PostQuitMessage(0);

		return;
	}

	m_cbDSN.SetCurSel(0);	
}

void CODBCBindingDlg::OnBnClickedButtonInsertToTable()
{
	// TODO: Add your control notification handler code here
	CString strDSN;
	m_cbDSN.GetLBText(m_cbDSN.GetCurSel(), strDSN);

	UpdateData(TRUE);
	// Check if user has copied the text from file kanji.txt first
	// If not, do not proceed
	if(m_strContent.GetLength() == 0)
	{
		MessageBox(_T("Please INSERT 2000 JPN characters!"));
		return;
	}

	SQLHENV henv;
	if(SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv))
	{
		if(SQL_SUCCESS == SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER))
      {
			SQLHDBC hdbc;
			if(SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc))
			{
				if (SQL_SUCCESS != SQLSetConnectOption(hdbc, 1041, (UDWORD) "IVSE.LIC"))
				{
					throw CString(_T("Problem in Setting the IVSE.LIC"));
				}
				if (SQL_SUCCESS != SQLSetConnectOption(hdbc, 1042, (UDWORD) "SVc5NzA3a01nSnNSdEJ3VHNK" ))
				{
					throw CString(_T("Problem in Setting the LICENCE key"));

				}

				if(MySQLSuccess(SQLConnect(hdbc, (SQLTCHAR*)(LPCTSTR)strDSN, SQL_NTS, (SQLTCHAR*)(LPCTSTR)m_strUsername, SQL_NTS, (SQLTCHAR*)(LPCTSTR)m_strPassword, SQL_NTS)))
				{
					SQLHSTMT hstmt = NULL;
					if(SQL_SUCCESS == SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt))
					{
						SQLRETURN retcode;

						CString strSQL;						

					   TCHAR *fromBuffer = m_strContent.GetBuffer();
                  CString msg;
						msg.Format(_T("the length is %d"), _tcslen(fromBuffer));
                  MessageBox(msg);

						strSQL = _T("insert into test_varchar2000 values(?);");
                  //strSQL = _T("update test_varchar2000 set vol = ?;");

						retcode = SQLPrepare(hstmt, (SQLTCHAR*)(LPCTSTR)strSQL, SQL_NTS);

						SQLLEN cbData1(SQL_NTS);

						retcode = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_TCHAR, SQL_VARCHAR, 2000, 0, fromBuffer, (2000+1) * sizeof(TCHAR), &cbData1);
						

						retcode = SQLExecute(hstmt);						

						if(MySQLSuccess(retcode))
						{
							MessageBox(_T("Successfully inserted!"));
						}
						else
						{
							SQLTCHAR text[1024], state[16];
							SQLSMALLINT cbTextLen(SQL_NTS);
							SQLINTEGER nNativeError;

							retcode = SQLGetDiagRec(SQL_HANDLE_STMT,
								hstmt,
								1,
								state,
								&nNativeError,
								text,
								1024,
								&cbTextLen);

							CString s;
							s.Format(_T("%s, %s\n"), state, text);

							MessageBox(s);
						}

						
					}

					SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
					SQLDisconnect(hdbc);
				}
				else
				{
					SQLTCHAR text[1024], state[16];
					SQLSMALLINT cbTextLen(SQL_NTS);
					SQLINTEGER nNativeError;

					SQLGetDiagRec(SQL_HANDLE_DBC,
						hdbc,
						1,
						state,
						&nNativeError,
						text,
						1024,
						&cbTextLen);

					CString s;
					s.Format(_T("Failed to connect!\n%s"), text);

					MessageBox(s);
				}

				SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
			}
		}

		SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}
}

