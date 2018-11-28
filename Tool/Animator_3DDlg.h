#pragma once

#include "ComponentDlg.h"
#include "afxwin.h"

// CAnimator_3DDlg ��ȭ �����Դϴ�.

struct tAnimationInfo;

class CAnimator_3DDlg : public CComponentDlg
{
	DECLARE_DYNAMIC(CAnimator_3DDlg)

public:
	CAnimator_3DDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAnimator_3DDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATOR_3DDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
	CComboBox							m_cbAnimation;
	CComboBox							m_cbMotionBlur;
	const map<wstring, tAnimationInfo>*	m_mapAnim;
	bool								m_bFocusCB;
	bool								m_bFocusBlur;

public:
	virtual void UpdateComponent(CGameObject* _pObj);
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnSetfocus();
	afx_msg void OnCbnKillfocus();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDeleteAnimation();

	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSetfocusCombo3();
	afx_msg void OnCbnKillfocusCombo3();
};
