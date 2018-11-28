#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CTextDlg 대화 상자입니다.

class CTextDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTextDlg)

public:
	CTextDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTextDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEXTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CEdit		m_editSize;
	CEdit		m_editColR;
	CEdit		m_editColG;
	CEdit		m_editColB;
	CEdit		m_editText;

	bool		m_bForcusSize;
	bool		m_bForcusColR;
	bool		m_bForcusColG;
	bool		m_bForcusColB;
	bool		m_bForcusText;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();

private:
	afx_msg void OnEnChangeSize();
	afx_msg void OnEnChangeColR();
	afx_msg void OnEnChangeColG();
	afx_msg void OnEnChangeColB();
	afx_msg void OnEnChangeText();
	afx_msg void OnEnSetfocusSize();
	afx_msg void OnEnKillfocusSize();
	afx_msg void OnEnSetfocusText();
	afx_msg void OnEnKillfocusText();
	afx_msg void OnEnSetfocusColorR();
	afx_msg void OnEnKillfocusColorR();
	afx_msg void OnEnSetfocusColorG();
	afx_msg void OnEnKillfocusColorG();
	afx_msg void OnEnSetfocusColorB();
	afx_msg void OnEnKillfocusColorB();
};
