#pragma once

#include "ComponentDlg.h"

// CTrailDlg 대화 상자입니다.

class CTrailDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CTrailDlg)

public:
	CTrailDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CTrailDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRAILDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
