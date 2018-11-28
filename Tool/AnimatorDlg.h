#pragma once


// CAnimatorDlg dialog

#include "ComponentDlg.h"
#include "afxwin.h"
#include "Animator_2D.h"

class CAnimatorDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CAnimatorDlg)

public:
	CAnimatorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnimatorDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATORDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_bRepeat;
	CComboBox	m_cbAnim;
	bool		m_bFocus_cb;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	vector<tAnimPair> m_vecAnim;
	afx_msg void OnAddBnClicked();
	afx_msg void OnCbnSetfocus();
	afx_msg void OnCbnKillfocus();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnCbnSelchange();
	virtual BOOL OnInitDialog();
};
