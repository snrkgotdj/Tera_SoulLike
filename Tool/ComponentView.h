#pragma once
#include "afxwin.h"



// CComponentView form view

class CComponentDlg;
class CGameObject;

class CComponentView : public CFormView
{
	DECLARE_DYNCREATE(CComponentView)

protected:
	CComponentView();           // protected constructor used by dynamic creation
	virtual ~CComponentView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_COMPONENTVIEW };
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
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	
private:
	CGameObject*	m_pTargetObj;
	CComponentDlg*	m_arrDlg[(UINT)COMPONENT_TYPE::END];
	CComponentDlg*	m_pObjInfoDlg;
	int				m_iStartY;

public:
	void SetTarget(CGameObject* _pTarget) { m_pTargetObj = _pTarget; }
	void InitTarget();
	void Init(CGameObject * _pObj);
	CComponentDlg* GetTransformDlg() { return m_arrDlg[(UINT)COMPONENT_TYPE::TRANSFORM]; }

public:
	void UpdateComponent();
	void Update();
	CButton m_bnAC;
	afx_msg void OnAddComBnClicked();
	virtual void OnInitialUpdate();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


