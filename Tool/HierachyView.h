#pragma once
#include "afxcmn.h"

enum TREECTRLDLG
{
	HIERARCHY,
	RES,
	END
};

// CHierachyView form view

class CHierachyDlg;
class CResDlg;

class CHierachyView : public CFormView
{
	DECLARE_DYNCREATE(CHierachyView)

protected:
	CHierachyView();           // protected constructor used by dynamic creation
	virtual ~CHierachyView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HIERACHYVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CHierachyDlg*	m_pHierachyDlg;
	CResDlg*		m_pResDlg;

public:
	void Init(TREECTRLDLG _eTreeCtrlDlg = TREECTRLDLG::END);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
};


