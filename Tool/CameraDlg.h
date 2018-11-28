#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CCameraDlg 대화 상자입니다.

class CCameraDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CCameraDlg)

public:
	CCameraDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCameraDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMERADLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	BOOL		m_btnPerspective;
	CEdit		m_editWidth;
	CEdit		m_editHeght;
	CEdit		m_editScale;
	CEdit		m_editNear;
	CEdit		m_editFar;
	CEdit		m_editFilter;
	CComboBox	m_cbLayerList;
	CListBox	m_lbCheckLayer;
private:
	afx_msg void OnBnClicked();	
	afx_msg void OnEnChangeWidth();
	afx_msg void OnEnChangeHeight();
	afx_msg void OnEnChangeScale();
	afx_msg void OnEnChangeNear();
	afx_msg void OnEnChangeFar();
	afx_msg void OnBnFilter();

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	afx_msg void OnCbnSelchange();
	afx_msg void OnBnRelease();
	virtual void PostNcDestroy();
};
