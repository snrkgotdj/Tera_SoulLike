#pragma once
#include "ComponentDlg.h"
#include "afxwin.h"

// CObjectInfoDlg ��ȭ �����Դϴ�.

class CObjectInfoDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CObjectInfoDlg)

public:
	CObjectInfoDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CObjectInfoDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTINFODLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
