// ODBC BindingDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CODBCBindingDlg dialog
class CODBCBindingDlg : public CDialog
{
// Construction
public:
	CODBCBindingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ODBCBINDING_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbDSN;
	CString m_strUsername;
	CString m_strPassword;
	CString m_strContent;
	afx_msg void OnBnClickedButtonInsertToTable();
private:
	void LoadSysDSNs(void);
};
