#pragma once


// CTransformDlg dialog

#include "ComponentDlg.h"
#include "afxwin.h"

class CTransformDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTransformDlg)

public:
	CTransformDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransformDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSFORMDLG };
#endif

public:
	virtual void UpdateComponent(CGameObject* _pObj);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	CEdit   m_arrEdit[3][3];
	bool    m_bFocus[3][3];

	afx_msg void OnEnSetfocus(UINT _id);
	afx_msg void OnEnKillfocus(UINT _id);
	afx_msg void OnEnChange(UINT _id);
	BOOL m_bCull;
	afx_msg void OnBnClickedCull();
};
