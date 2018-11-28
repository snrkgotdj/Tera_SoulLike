#pragma once


// CScriptDlg dialog

#include "ComponentDlg.h"
#include "afxwin.h"

class CScriptDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CScriptDlg)

public:
	CScriptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CScriptDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCRIPTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

private:
	CComboBox m_cbScript;
	CListBox m_List;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();

	afx_msg void OnAddScript();
	virtual BOOL OnInitDialog();

	void UpdateList();
	afx_msg void OnBnClickedDelete();
};
