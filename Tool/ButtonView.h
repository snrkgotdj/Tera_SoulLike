#pragma once
#include "afxwin.h"



// CButtonView form view

class CGameObject;

class CButtonView : public CFormView
{
	DECLARE_DYNCREATE(CButtonView)

protected:
	CButtonView();           // protected constructor used by dynamic creation
	virtual ~CButtonView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BUTTONVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	bool				m_bPlay;
	CGameObject*		m_pCamObj;
	CGameObject*		m_pGridObj;
	list<CGameObject*>	m_listRTObj;

	CButton				m_btnPlay;
	CButton				m_btnPause;
	CButton				m_btnStop;
public:
	void Update();

private:
	void CreateToolObj();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPlayBtn();
	afx_msg void OnPauseBtn();
	afx_msg void OnStopBtn();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


