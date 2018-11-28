// ColliderDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ColliderDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "GameObject.h"
#include "Collider.h"

// CColliderDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CColliderDlg, CDialogEx)

CColliderDlg::CColliderDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_COLLIDERDLG, pParent)
	, m_arrFocusPos{ false, }
	, m_arrFocusScale{ false, }
{

}

CColliderDlg::~CColliderDlg()
{
}

void CColliderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	int iStartID = IDC_EDIT1;

	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrOffsetPos[i]);
	}

	iStartID = IDC_EDIT4;

	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrOffsetScale[i]);
	}
	DDX_Control(pDX, IDC_COMBO1, m_cbType);
}

BOOL CColliderDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_cbType.InsertString((UINT)COLLIDER_TYPE::SPHERE, L"Sphere");
	m_cbType.InsertString((UINT)COLLIDER_TYPE::CUBE, L"Cube");
	m_cbType.InsertString((UINT)COLLIDER_TYPE::OBB, L"Obb");

	return TRUE;
}

void CColliderDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj || !m_bChange)
		return;

	CCollider* pColl = m_pTargetObj->Collider();

	Vec3 vOffsetPos = pColl->GetOffsetPos();
	Vec3 vOffsetScale= pColl->GetOffsetScale();

	CString str;
	for (int i = 0; i < 3; ++i)
	{
		if (false == m_arrFocusPos[i])
		{
			str.Format(L"%.2f", vOffsetPos[i]);
			m_arrOffsetPos[i].SetWindowTextW(str);
		}

		if (false == m_arrFocusScale[i])
		{
			str.Format(L"%.2f", vOffsetScale[i]);
			m_arrOffsetScale[i].SetWindowTextW(str);
		}
	}

	m_cbType.SetCurSel((UINT)m_pTargetObj->Collider()->GetColType());

	m_bChange = false;
}


BEGIN_MESSAGE_MAP(CColliderDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CColliderDlg::OnBnClickedDeleteThis)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT1, IDC_EDIT3, &CColliderDlg::OnEnChangeOffsetPos)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT4, IDC_EDIT6, &CColliderDlg::OnEnChangeOffsetScale)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CColliderDlg::OnCbnSelchangeType)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT1, IDC_EDIT3, &CColliderDlg::OnEnSetfocusPos)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT1, IDC_EDIT3, &CColliderDlg::OnEnKillfocusPos)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_EDIT4, IDC_EDIT6, &CColliderDlg::OnEnSetfocusScale)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT4, IDC_EDIT6, &CColliderDlg::OnEnKillfocusScale)
END_MESSAGE_MAP()


// CColliderDlg 메시지 처리기입니다.


void CColliderDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}


void CColliderDlg::OnBnClickedDeleteThis()
{
	m_pTargetObj->DeleteComponent(COMPONENT_TYPE::COLLIDER);
}


void CColliderDlg::OnEnChangeOffsetPos(UINT _id)
{
	int iID = _id - IDC_EDIT1;

	CString str;
	m_arrOffsetPos[iID].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());	

	Vec3 vPos = m_pTargetObj->Collider()->GetOffsetPos();

	// POSITION
	if (iID % 3 == 0)
	{
		vPos.x = f;
	}
	else if (iID % 3 == 1)
	{
		vPos.y = f;
	}
	else if(iID % 3 == 2)
	{
		vPos.z = f;
	}

	m_pTargetObj->Collider()->SetOffsetPos(vPos);

	m_bChange = true;
}

void CColliderDlg::OnEnChangeOffsetScale(UINT _id)
{
	int iID = _id - IDC_EDIT4;

	CString str;
	m_arrOffsetScale[iID].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	Vec3 vSacle = m_pTargetObj->Collider()->GetOffsetScale();

	// POSITION
	if (iID % 3 == 0)
	{
		vSacle.x = f;
	}
	else if (iID % 3 == 1)
	{
		vSacle.y= f;
	}
	else if (iID % 3 == 2)
	{
		vSacle.z = f;
	}

	m_pTargetObj->Collider()->SetOffsetScale(vSacle);

	m_bChange = true;
}


void CColliderDlg::OnCbnSelchangeType()
{

	int iCurSel = m_cbType.GetCurSel();

	if (-1 == iCurSel)
	{
		return;
	}

	m_pTargetObj->Collider()->SetColType((COLLIDER_TYPE)iCurSel);

	::SetFocus(m_hWndOwner);

	m_bChange = true;
}

void CColliderDlg::OnEnSetfocusPos(UINT _id)
{
	int iID = _id - IDC_EDIT1;
	m_arrFocusPos[iID % 3] = true;
}

void CColliderDlg::OnEnKillfocusPos(UINT _id)
{
	int iID = _id - IDC_EDIT1;
	m_arrFocusPos[iID % 3] = false;
}

void CColliderDlg::OnEnSetfocusScale(UINT _id)
{
	int iID = _id - IDC_EDIT4;
	m_arrFocusScale[iID % 3] = true;
}

void CColliderDlg::OnEnKillfocusScale(UINT _id)
{
	int iID = _id - IDC_EDIT4;
	m_arrFocusScale[iID % 3] = false;
}