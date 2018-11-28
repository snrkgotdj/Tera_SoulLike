#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CLightDlg 대화 상자입니다.

class CLightDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CLightDlg)

public:
	CLightDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLightDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LIGHTDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	
private:
	CComboBox	m_cbType;
	CEdit		m_arrColEdit[3][3];
	bool		m_bColFocus[3][3];
	CEdit		m_arrPosEdit[3];
	bool		m_bPosFocus[3];
	CEdit		m_arrDirEdit[3];
	bool		m_bDirFocus[3];
	CEdit		m_editRange;
	bool		m_bRangeFocus;
	CEdit		m_arrAngleEdit[2];
	bool		m_bAngleFocus[2];

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnSetfocusCol(UINT _iID);
	afx_msg void OnEnKillfocusCol(UINT _iID);
	afx_msg void OnEnChangeCol(UINT _iID);
	afx_msg void OnEnSetfocusPos(UINT _iID);
	afx_msg void OnEnKillfocusPos(UINT _iID);
	afx_msg void OnEnChangePos(UINT _iID);
	afx_msg void OnEnSetfocusDir(UINT _iID);
	afx_msg void OnEnKillfocusDir(UINT _iID);
	afx_msg void OnEnChangeDir(UINT _iID);
	afx_msg void OnEnSetfocusEdit();
	afx_msg void OnEnKillfocusEdit();
	afx_msg void OnEnChangeEdit();
	afx_msg void OnEnSetfocusAngle(UINT _iID);
	afx_msg void OnEnKillfocusAngle(UINT _iID);
	afx_msg void OnEnChangeAngle(UINT _iID);
	afx_msg void OnCbnSelchange();
};
