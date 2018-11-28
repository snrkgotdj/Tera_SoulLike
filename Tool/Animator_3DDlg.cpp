// Animator_3DDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Animator_3DDlg.h"
#include "afxdialogex.h"
#include "Tool.h"

#include "AddAnimation_3DDlg.h"

#include "GameObject.h"
#include "Animator_3D.h"

// CAnimator_3DDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAnimator_3DDlg, CDialogEx)

CAnimator_3DDlg::CAnimator_3DDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_ANIMATOR_3DDLG, pParent)
	, m_bFocusCB(false)
	, m_bFocusBlur(false)
{

}

CAnimator_3DDlg::~CAnimator_3DDlg()
{
}

void CAnimator_3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbAnimation);
	DDX_Control(pDX, IDC_COMBO3, m_cbMotionBlur);
}

void CAnimator_3DDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj)
		return;

	CAnimator_3D* pAnim = m_pTargetObj->Animator_3D();

	if (false == m_bFocusCB)
	{
		m_mapAnim = pAnim->GetAnimationInfoMap();
		m_cbAnimation.ResetContent();

		map<wstring, tAnimationInfo>::const_iterator iter = m_mapAnim->begin();
		UINT iIdx = 0;

		m_cbAnimation.InsertString(iIdx++, L"None");
		for (; iter != m_mapAnim->end(); ++iter)
		{
			m_cbAnimation.InsertString(iIdx++, iter->first.c_str());
		}

		if (pAnim->GetCurAnimationKey().empty())
			m_cbAnimation.SetCurSel(0);
		else
		{
			int iIdx = m_cbAnimation.FindString(0, pAnim->GetCurAnimationKey().c_str());
			m_cbAnimation.SetCurSel(iIdx);
		}
	}

	if (false == m_bFocusBlur)
	{
		int iBlurFrame = pAnim->GetBlurFrame();
		m_cbMotionBlur.SetCurSel(iBlurFrame);
	}


	m_bChange = false;
}

BEGIN_MESSAGE_MAP(CAnimator_3DDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAnimator_3DDlg::OnBnClickedDelete)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAnimator_3DDlg::OnCbnSelchange)
	ON_CBN_SETFOCUS(IDC_COMBO1, &CAnimator_3DDlg::OnCbnSetfocus)
	ON_CBN_KILLFOCUS(IDC_COMBO1, &CAnimator_3DDlg::OnCbnKillfocus)
	ON_BN_CLICKED(IDC_BUTTON2, &CAnimator_3DDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_BUTTON3, &CAnimator_3DDlg::OnBnClickedDeleteAnimation)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CAnimator_3DDlg::OnCbnSelchangeCombo3)
	ON_CBN_SETFOCUS(IDC_COMBO3, &CAnimator_3DDlg::OnCbnSetfocusCombo3)
	ON_CBN_KILLFOCUS(IDC_COMBO3, &CAnimator_3DDlg::OnCbnKillfocusCombo3)
END_MESSAGE_MAP()


// CAnimator_3DDlg 메시지 처리기입니다.


void CAnimator_3DDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}


void CAnimator_3DDlg::OnBnClickedDelete()
{
	m_pTargetObj->DeleteComponent(COMPONENT_TYPE::ANIMATOR_3D);
}


void CAnimator_3DDlg::OnCbnSelchange()
{
	int iIdx = m_cbAnimation.GetCurSel();
	CString str;
	m_cbAnimation.GetLBText(iIdx, str);
	m_pTargetObj->Animator_3D()->PlayAnimation(str.GetBuffer());

	m_bChange = true;
}


void CAnimator_3DDlg::OnCbnSetfocus()
{
	m_bFocusCB = true;
}


void CAnimator_3DDlg::OnCbnKillfocus()
{
	m_bFocusCB = false;
}


void CAnimator_3DDlg::OnBnClickedAdd()
{
	CAddAnimation_3DDlg dlg;
	dlg.SetTarGet(m_pTargetObj);
	dlg.DoModal();

	m_bChange = true;
}

void CAnimator_3DDlg::OnBnClickedDeleteAnimation()
{
	int iIdx = m_cbAnimation.GetCurSel();
	if (0 == iIdx)
		return;

	CString str;
	m_cbAnimation.GetLBText(iIdx, str);
	m_pTargetObj->Animator_3D()->DeleteAnimation(str.GetBuffer());

	m_bChange = true;
}


BOOL CAnimator_3DDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_cbMotionBlur.InsertString(0, L"0");
	m_cbMotionBlur.InsertString(1, L"1");
	m_cbMotionBlur.InsertString(2, L"2");
	m_cbMotionBlur.InsertString(3, L"3");
	m_cbMotionBlur.InsertString(4, L"4");
	m_cbMotionBlur.InsertString(5, L"5");
	m_cbMotionBlur.InsertString(6, L"6");
	m_cbMotionBlur.InsertString(7, L"7");
	m_cbMotionBlur.InsertString(8, L"8");
	m_cbMotionBlur.InsertString(9, L"9");

	return TRUE;  
}


void CAnimator_3DDlg::OnCbnSelchangeCombo3()
{
	int iIdx = m_cbMotionBlur.GetCurSel();

	m_pTargetObj->Animator_3D()->SetBlur((bool)iIdx, iIdx);

	m_bChange = true;
}


void CAnimator_3DDlg::OnCbnSetfocusCombo3()
{
	m_bFocusBlur = true;
}


void CAnimator_3DDlg::OnCbnKillfocusCombo3()
{
	m_bFocusBlur = false;
}
