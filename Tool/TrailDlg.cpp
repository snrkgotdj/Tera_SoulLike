// TrailDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "TrailDlg.h"
#include "afxdialogex.h"
#include "Tool.h"

#include "GameObject.h"
#include "Trail.h"

// CTrailDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CTrailDlg, CDialogEx)

CTrailDlg::CTrailDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_TRAILDLG, pParent)
	, m_arrFocusTop{false, }
	, m_arrFocusBottom{false, }
{

}

CTrailDlg::~CTrailDlg()
{
}

void CTrailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	int iStartTopID = IDC_TOP_X;
	int iStartBottomID = IDC_BOTTOM_X;

	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, iStartTopID++, m_arrTop[i]);
		DDX_Control(pDX, iStartBottomID++, m_arrBottom[i]);
	}
}


BEGIN_MESSAGE_MAP(CTrailDlg, CDialogEx)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_TOP_X, IDC_TOP_Z, &CTrailDlg::OnEnChangeTop)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_BOTTOM_X, IDC_BOTTOM_Z, &CTrailDlg::OnEnChangeBottom)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_TOP_X, IDC_TOP_Z, &CTrailDlg::OnEnSetfocusTop)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_TOP_X, IDC_TOP_Z, &CTrailDlg::OnEnKillfocusTop)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_BOTTOM_X, IDC_BOTTOM_Z, &CTrailDlg::OnEnSetfocusBottom)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_BOTTOM_X, IDC_BOTTOM_Z, &CTrailDlg::OnEnKillfocusBottom)
END_MESSAGE_MAP()


// CTrailDlg 메시지 처리기입니다.


void CTrailDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (m_pTargetObj == NULL || !m_bChange)
		return;

	CString str;
	Vec3 vPos;

	for (int i = 0; i < 3; ++i)
	{
		if (false == m_arrFocusTop[i])
		{
			vPos = m_pTargetObj->Trail()->GetPos(0);
			str.Format(L"%.2f", vPos[i]);
			m_arrTop[i].SetWindowTextW(str);
		}

		if (false == m_arrFocusBottom[i])
		{
			vPos = m_pTargetObj->Trail()->GetPos(1);
			str.Format(L"%.2f", vPos[i]);
			m_arrBottom[i].SetWindowTextW(str);
		}
	}

	m_bChange = false;
}

void CTrailDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}

void CTrailDlg::OnEnChangeTop(UINT _id)
{
	int iID = _id - IDC_TOP_X;

	CString str;
	m_arrTop[iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	Vec3 vPos = m_pTargetObj->Trail()->GetPos(0);
	vPos[iID % 3] = f;
	m_pTargetObj->Trail()->SetPos(0, vPos);

	m_bChange = true;
}

void CTrailDlg::OnEnChangeBottom(UINT _id)
{
	int iID = _id - IDC_BOTTOM_X;

	CString str;
	m_arrBottom[iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	Vec3 vPos = m_pTargetObj->Trail()->GetPos(1);
	vPos[iID % 3] = f;
	m_pTargetObj->Trail()->SetPos(1, vPos);

	m_bChange = true;
}


void CTrailDlg::OnEnSetfocusTop(UINT _id)
{
	int iID = _id - IDC_TOP_X;
	m_arrFocusTop[iID % 3] = true;
}

void CTrailDlg::OnEnKillfocusTop(UINT _id)
{
	int iID = _id - IDC_TOP_X;
	m_arrFocusTop[iID % 3] = false;
}

void CTrailDlg::OnEnSetfocusBottom(UINT _id)
{
	int iID = _id - IDC_BOTTOM_X;
	m_arrFocusBottom[iID % 3] = true;
}

void CTrailDlg::OnEnKillfocusBottom(UINT _id)
{
	int iID = _id - IDC_BOTTOM_X;
	m_arrFocusBottom[iID % 3] = false;
}
