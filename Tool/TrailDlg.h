#pragma once

#include "ComponentDlg.h"

// CTrailDlg ��ȭ �����Դϴ�.

class CTrailDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTrailDlg)

public:
	CTrailDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTrailDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAILDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CEdit   m_arrTop[3];
	CEdit   m_arrBottom[3];
	bool	m_arrFocusTop[3];
	bool	m_arrFocusBottom[3];

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();

	void OnEnChangeTop(UINT _id);
	void OnEnChangeBottom(UINT _id);
	void OnEnSetfocusTop(UINT _id);
	void OnEnKillfocusTop(UINT _id);
	void OnEnSetfocusBottom(UINT _id);
	void OnEnKillfocusBottom(UINT _id);
};
