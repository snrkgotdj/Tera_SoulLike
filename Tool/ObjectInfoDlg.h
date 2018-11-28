#pragma once
#include "ComponentDlg.h"
#include "afxwin.h"

// CObjectInfoDlg 대화 상자입니다.

class CObjectInfoDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CObjectInfoDlg)

public:
	CObjectInfoDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectInfoDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTINFODLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_bActive;
	bool		m_arrFocus[2];
	CEdit		m_editObjName;
	CComboBox	m_cbLayer;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	afx_msg void OnActiveBtn();

	afx_msg void OnEnSetfocus(UINT _id);
	afx_msg void OnEnKillfocus(UINT _id);
	afx_msg void OnEnChangeTag();
	afx_msg void OnCbnSelchangeLayer();
};
