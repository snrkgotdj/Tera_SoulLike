#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CColliderDlg 대화 상자입니다.

class CColliderDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CColliderDlg)

public:
	CColliderDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CColliderDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COLLIDERDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CEdit		m_arrOffsetPos[3];
	CEdit		m_arrOffsetScale[3];
	CComboBox	m_cbType;

	bool		m_arrFocusPos[3];
	bool		m_arrFocusScale[3];

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedDeleteThis();
	afx_msg void OnEnChangeOffsetPos(UINT _id);
	afx_msg void OnEnChangeOffsetScale(UINT _id);
	afx_msg void OnCbnSelchangeType();
	virtual BOOL OnInitDialog();

	afx_msg void OnEnSetfocusPos(UINT _id);
	afx_msg void OnEnKillfocusPos(UINT _id);
	afx_msg void OnEnSetfocusScale(UINT _id);
	afx_msg void OnEnKillfocusScale(UINT _id);
};
