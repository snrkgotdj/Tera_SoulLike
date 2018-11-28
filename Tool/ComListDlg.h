#pragma once
#include "afxwin.h"


// CComListDlg dialog

class CComListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CComListDlg)

public:
	CComListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CComListDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMLISTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cbCom;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
};
