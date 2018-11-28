// AnimatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "AnimatorDlg.h"
#include "afxdialogex.h"

#include "Animation.h"

#include "GameObject.h"

#include "AddAnimDlg.h"

// CAnimatorDlg dialog

IMPLEMENT_DYNAMIC(CAnimatorDlg, CDialogEx)

CAnimatorDlg::CAnimatorDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_ANIMATORDLG, pParent)
	, m_bRepeat(FALSE)
	, m_bFocus_cb(false)	
{
}

CAnimatorDlg::~CAnimatorDlg()
{
}

void CAnimatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bRepeat);
	DDX_Control(pDX, IDC_COMBO1, m_cbAnim);
}

void CAnimatorDlg::UpdateComponent(CGameObject* _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj)
		return;

	if (!m_bFocus_cb)
	{
		m_vecAnim.clear();
		m_cbAnim.ResetContent();
		m_pTargetObj->Animator_2D()->GetAnimations(m_vecAnim);

		for (UINT i = 0; i < m_vecAnim.size(); ++i)
		{
			m_cbAnim.InsertString(i, m_vecAnim[i].strKey.c_str());
		}

		CAnimation* pCurAnim = m_pTargetObj->Animator_2D()->GetCurAnim();

		if (NULL == pCurAnim)
			m_cbAnim.SetCurSel(-1);
		else
		{
			int iIdx = m_cbAnim.FindString(0, pCurAnim->GetKey().c_str());
			m_cbAnim.SetCurSel(iIdx);
		}		
	}	

	m_bRepeat = m_pTargetObj->Animator_2D()->IsRepeat();
	UpdateData(false);
}

BEGIN_MESSAGE_MAP(CAnimatorDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAnimatorDlg::OnAddBnClicked)
	
	ON_CBN_SETFOCUS(IDC_COMBO1, &CAnimatorDlg::OnCbnSetfocus)
	ON_CBN_KILLFOCUS(IDC_COMBO1, &CAnimatorDlg::OnCbnKillfocus)
	ON_BN_CLICKED(IDC_CHECK1, &CAnimatorDlg::OnBnClickedCheck)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAnimatorDlg::OnCbnSelchange)
END_MESSAGE_MAP()


// CAnimatorDlg message handlers
void CAnimatorDlg::OnAddBnClicked()
{
	CAddAnimDlg dlg;

	dlg.m_pTarget = m_pTargetObj;

	dlg.DoModal();
}

void CAnimatorDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CComponentDlg::PostNcDestroy();
}

void CAnimatorDlg::OnCbnSetfocus()
{
	m_bFocus_cb = true;
}

void CAnimatorDlg::OnCbnKillfocus()
{
	m_bFocus_cb = false;
}


void CAnimatorDlg::OnBnClickedCheck()
{
	if (m_bRepeat)
		m_bRepeat = false;
	else
		m_bRepeat = true;

	UpdateData(false);

	m_pTargetObj->Animator_2D()->SetRepeat(m_bRepeat != 0);
}


void CAnimatorDlg::OnCbnSelchange()
{
	int iIdx = m_cbAnim.GetCurSel();
	CString str;
	m_cbAnim.GetLBText(iIdx, str);
	m_pTargetObj->Animator_2D()->PlayAnimation(str.GetBuffer(), m_bRepeat != 0);
}

BOOL CAnimatorDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();	

	return TRUE;  
}
