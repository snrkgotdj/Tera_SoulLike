// TransformDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "TransformDlg.h"
#include "afxdialogex.h"

#include "GameObject.h"
#include "Transform.h"

// CTransformDlg dialog

IMPLEMENT_DYNAMIC(CTransformDlg, CDialogEx)

CTransformDlg::CTransformDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_TRANSFORMDLG, pParent)
	, m_bCull(FALSE)
{
}

CTransformDlg::~CTransformDlg()
{
}

void CTransformDlg::UpdateComponent(CGameObject * _pObj)
{
	if (NULL == _pObj)
		return;

	m_pTargetObj = _pObj;

	CTransform* pTrans = m_pTargetObj->Transform();
	Vec3 vTransform[3] = { pTrans->GetLocalPos(),  pTrans->GetLocalRot(), pTrans->GetLocalScale() };

	CString str;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			if (!m_bFocus[i][j])
			{
				str.Format(L"%.2f", vTransform[i][j]);
				m_arrEdit[i][j].SetWindowTextW(str);
			}
		}
	}

	m_bCull = (pTrans->IsCull() != 0);
	UpdateData(false);

	m_bChange = false;
}

void CTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	int iStartID = IDC_POS_X;

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			DDX_Control(pDX, iStartID++, m_arrEdit[i][j]);
		}
	}
	DDX_Check(pDX, IDC_CHECK2, m_bCull);
}


BEGIN_MESSAGE_MAP(CTransformDlg, CDialogEx)	
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_POS_X, IDC_SCA_Z, &CTransformDlg::OnEnSetfocus)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_POS_X, IDC_SCA_Z, &CTransformDlg::OnEnKillfocus)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_POS_X, IDC_SCA_Z, &CTransformDlg::OnEnChange)	
	ON_BN_CLICKED(IDC_CHECK2, &CTransformDlg::OnBnClickedCull)
END_MESSAGE_MAP()


// CTransformDlg message handlers


void CTransformDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;

	CComponentDlg::PostNcDestroy();
}


void CTransformDlg::OnEnSetfocus(UINT _id)
{	
	int iID = _id - 1001;
	m_bFocus[iID / 3][iID % 3] = true;
}

void CTransformDlg::OnEnKillfocus(UINT _id)
{
	int iID = _id - 1001;
	m_bFocus[iID / 3][iID % 3] = false;
}

void CTransformDlg::OnEnChange(UINT _id)
{
	int iID = _id - 1001;

	if (false == m_bFocus[iID / 3][iID % 3])
		return;

	CString str;
	m_arrEdit[iID / 3][iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	// POSITION
	if (iID / 3 == 0)
	{
		Vec3 vPos = m_pTargetObj->Transform()->GetLocalPos();
		vPos[iID % 3] = f;
		m_pTargetObj->Transform()->SetLocalPos(vPos);
	}
	// ROTATION
	else if (iID / 3 == 1)
	{
		Vec3 vRot = m_pTargetObj->Transform()->GetLocalRot();
		vRot[iID % 3] = f;
		m_pTargetObj->Transform()->SetLocalRot(vRot);
	}
	// SCALE
	else
	{
		if (0.f == f)
			return;

		Vec3 vScale = m_pTargetObj->Transform()->GetLocalScale();
		vScale[iID % 3] = f;
		m_pTargetObj->Transform()->SetLocalScale(vScale);
	}	

	m_bChange = true;
}

void CTransformDlg::OnBnClickedCull()
{
	if (m_bCull)
	{
		m_bCull = false;
	}
	else
	{
		m_bCull = true;
	}

	UpdateData(false);

	if (NULL != m_pTargetObj)
		m_pTargetObj->Transform()->SetCamCull(m_bCull != 0);

}
