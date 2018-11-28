// LightDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LightDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "MainFrm.h"
#include "ComponentView.h"
#include "TransformDlg.h"

#include "GameObject.h"
#include "Light.h"

// CLightDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLightDlg, CDialogEx)

CLightDlg::CLightDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_LIGHTDLG, pParent)
	, m_bRangeFocus(false)
{

}

CLightDlg::~CLightDlg()
{
}

void CLightDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbType);

	int iStartID = IDC_DIFF_R;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			DDX_Control(pDX, iStartID++, m_arrColEdit[i][j]);
		}
	}

	iStartID = IDC_POS_X;
	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrPosEdit[i]);
	}

	iStartID = IDC_DIR_X;
	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrDirEdit[i]);
	}

	DDX_Control(pDX, IDC_RANGE, m_editRange);

	iStartID = IDC_ANGLE_OUT;
	for (int i = 0; i < 2; ++i)
	{
		DDX_Control(pDX, iStartID++, m_arrAngleEdit[i]);
	}
}

void CLightDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (m_pTargetObj == NULL || !m_bChange)
		return;

	tLightInfo tInfo = m_pTargetObj->Light()->GetLightInfo();
	m_cbType.SetCurSel(tInfo.iLightType);

	CString str;

	Vec3 vDiff = Vec3(tInfo.tCol.vDiff.x, tInfo.tCol.vDiff.y, tInfo.tCol.vDiff.z);
	for (int i = 0; i < 3; ++i)
	{
		if (!m_bColFocus[0][i])
		{
			str.Format(L"%.2f", vDiff[i]);
			m_arrColEdit[0][i].SetWindowTextW(str);
		}
	}

	Vec3 vSpec = Vec3(tInfo.tCol.vSpec.x, tInfo.tCol.vSpec.y, tInfo.tCol.vSpec.z);
	for (int i = 0; i < 3; ++i)
	{
		if (!m_bColFocus[1][i])
		{
			str.Format(L"%.2f", vSpec[i]);
			m_arrColEdit[1][i].SetWindowTextW(str);
		}
	}

	Vec3 vAmb = Vec3(tInfo.tCol.vAmb.x, tInfo.tCol.vAmb.y, tInfo.tCol.vAmb.z);
	for (int i = 0; i < 3; ++i)
	{
		if (!m_bColFocus[2][i])
		{
			str.Format(L"%.2f", vAmb[i]);
			m_arrColEdit[2][i].SetWindowTextW(str);
		}
	}

	Vec4 vTemp = tInfo.vLightPos;
	for (int i = 0; i < 3; ++i)
	{
		if (!m_bPosFocus[i])
		{
			Vec3 vPos = Vec3(vTemp.x, vTemp.y, vTemp.z);
			str.Format(L"%.2f", vPos[i]);
			m_arrPosEdit[i].SetWindowTextW(str);
		}
	}

	Vec3 vDir = tInfo.vLightDir;
	vDir.Normalize();
	for (int i = 0; i < 3; ++i)
	{
		if (!m_bDirFocus[i])
		{
			str.Format(L"%.2f", vDir[i]);
			m_arrDirEdit[i].SetWindowTextW(str);
		}
	}

	if (!m_bRangeFocus)
	{
		str.Format(L"%.2f", m_pTargetObj->Light()->GetRange());
		m_editRange.SetWindowTextW(str);
	}

	Vec2 vAngle = tInfo.vSpotAngle;
	for (int i = 0; i < 2; ++i)
	{
		if (!m_bAngleFocus[i])
		{
			float fTheta = 0.f;
			if (0 == i)
			{
				fTheta = acosf(vAngle.x) * (180.f / XM_PI);

			}
			else
			{
				fTheta = acosf(vAngle.y) * (180.f / XM_PI);
			}

			str.Format(L"%.2f", fTheta);
			m_arrAngleEdit[i].SetWindowTextW(str);
		}
	}

	m_bChange = false;

	CMainFrame* pMain = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CTransformDlg* pTrDlg = ((CTransformDlg*)((CComponentView*)pMain->GetComponentView())->GetTransformDlg());
	pTrDlg->SetChange(true);
}

BEGIN_MESSAGE_MAP(CLightDlg, CDialogEx)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_DIFF_R, IDC_AMBI_B, &CLightDlg::OnEnSetfocusCol)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_DIFF_R, IDC_AMBI_B, &CLightDlg::OnEnKillfocusCol)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_DIFF_R, IDC_AMBI_B, &CLightDlg::OnEnChangeCol)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_POS_X, IDC_POS_Z, &CLightDlg::OnEnSetfocusPos)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_POS_X, IDC_POS_Z, &CLightDlg::OnEnKillfocusPos)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_POS_X, IDC_POS_Z, &CLightDlg::OnEnChangePos)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_DIR_X, IDC_DIR_Z, &CLightDlg::OnEnSetfocusDir)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_DIR_X, IDC_DIR_Z, &CLightDlg::OnEnKillfocusDir)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_DIR_X, IDC_DIR_Z, &CLightDlg::OnEnChangeDir)

	ON_EN_SETFOCUS(IDC_RANGE, &CLightDlg::OnEnSetfocusEdit)
	ON_EN_KILLFOCUS(IDC_RANGE, &CLightDlg::OnEnKillfocusEdit)
	ON_EN_CHANGE(IDC_RANGE, &CLightDlg::OnEnChangeEdit)

	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_ANGLE_OUT, IDC_ANGLE_IN, &CLightDlg::OnEnSetfocusAngle)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_ANGLE_OUT, IDC_ANGLE_IN, &CLightDlg::OnEnKillfocusAngle)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_ANGLE_OUT, IDC_ANGLE_IN, &CLightDlg::OnEnChangeAngle)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CLightDlg::OnCbnSelchange)
END_MESSAGE_MAP()


// CLightDlg 메시지 처리기입니다.
void CLightDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}


BOOL CLightDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_cbType.InsertString(0, L"Directional");
	m_cbType.InsertString(1, L"Point");
	m_cbType.InsertString(2, L"Spot");

	return TRUE;
}


void CLightDlg::OnEnSetfocusCol(UINT _iID)
{
	int iID = _iID - 1020;
	m_bColFocus[iID / 3][iID % 3] = true;
}


void CLightDlg::OnEnKillfocusCol(UINT _iID)
{
	int iID = _iID - 1020;
	m_bColFocus[iID / 3][iID % 3] = false;
	m_bChange = true;
}


void CLightDlg::OnEnChangeCol(UINT _iID)
{
	int iID = _iID - 1020;

	if (false == m_bColFocus[iID / 3][iID % 3])
		return;

	CString str;
	m_arrColEdit[iID / 3][iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	// Diff
	if (iID / 3 == 0)
	{
		Vec4 vTemp = m_pTargetObj->Light()->GetColDiff();
		Vec3 vDiff = Vec3(vTemp.x, vTemp.y, vTemp.z);
		vDiff[iID % 3] = f;
		vTemp = Vec4(vDiff.x, vDiff.y, vDiff.z, 1.f);
		m_pTargetObj->Light()->SetColDiff(vTemp);
	}
	// Spec
	else if (iID / 3 == 1)
	{
		Vec4 vTemp = m_pTargetObj->Light()->GetColSpec();
		Vec3 vSpec = Vec3(vTemp.x, vTemp.y, vTemp.z);
		vSpec[iID % 3] = f;
		vTemp = Vec4(vSpec.x, vSpec.y, vSpec.z, 1.f);
		m_pTargetObj->Light()->SetColSpec(vTemp);
	}
	// Amb
	else
	{
		Vec4 vTemp = m_pTargetObj->Light()->GetColAmb();
		Vec3 vAmb = Vec3(vTemp.x, vTemp.y, vTemp.z);
		vAmb[iID % 3] = f;
		vTemp = Vec4(vAmb.x, vAmb.y, vAmb.z, 1.f);
		m_pTargetObj->Light()->SetColAmb(vTemp);
	}

	m_pTargetObj->Light()->SetRange(m_pTargetObj->Light()->m_fRange);
}


void CLightDlg::OnEnSetfocusPos(UINT _iID)
{
	int iID = _iID - 1001;
	m_bPosFocus[iID % 3] = true;
}


void CLightDlg::OnEnKillfocusPos(UINT _iID)
{
	int iID = _iID - 1001;
	m_bPosFocus[iID % 3] = false;
	m_bChange = true;
}


void CLightDlg::OnEnChangePos(UINT _iID)
{
	int iID = _iID - 1001;

	if (false == m_bPosFocus[iID % 3])
		return;

	CString str;
	m_arrPosEdit[iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	tLightInfo tInfo = m_pTargetObj->Light()->GetLightInfo();

	Vec4 vTemp = tInfo.vLightPos;
	Vec3 vPos = Vec3(vTemp.x, vTemp.y, vTemp.z);
	vPos[iID % 3] = f;
	vTemp = Vec4(vPos.x, vPos.y, vPos.z, 1.f);
	tInfo.vLightPos = vTemp;

	m_pTargetObj->Light()->SetLightInfo(tInfo);
}


void CLightDlg::OnEnSetfocusDir(UINT _iID)
{
	int iID = _iID - 1011;
	m_bDirFocus[iID % 3] = true;
}


void CLightDlg::OnEnKillfocusDir(UINT _iID)
{
	int iID = _iID - 1011;
	m_bDirFocus[iID % 3] = false;
	m_bChange = true;
}


void CLightDlg::OnEnChangeDir(UINT _iID)
{
	int iID = _iID - 1011;
	if (false == m_bDirFocus[iID % 3])
		return;

	CString str;
	m_arrDirEdit[iID % 3].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	tLightInfo tInfo = m_pTargetObj->Light()->GetLightInfo();

	if (0.f == f && 0.f == tInfo.vLightDir.x && 0.f == tInfo.vLightDir.y && 0.f == tInfo.vLightDir.z)
		return;

	tInfo.vLightDir.Normalize();
	tInfo.vLightDir[iID % 3] = f;
	if (tInfo.vLightDir.Distance() > 0)
		tInfo.vLightDir.Normalize();

	m_pTargetObj->Light()->SetLightInfo(tInfo);
	m_pTargetObj->Light()->SetRange(m_pTargetObj->Light()->m_fRange);
}


void CLightDlg::OnEnSetfocusEdit()
{
	m_bRangeFocus = true;
}


void CLightDlg::OnEnKillfocusEdit()
{
	m_bRangeFocus = false;
	m_bChange = true;
}


void CLightDlg::OnEnChangeEdit()
{
	if (false == m_bRangeFocus)
		return;

	CString str;
	m_editRange.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (f == 0.f)
		return;

	m_pTargetObj->Light()->SetRange(f);
	m_bChange = true;
}


void CLightDlg::OnEnSetfocusAngle(UINT _iID)
{
	int iID = _iID - 1029;
	m_bAngleFocus[iID % 2] = true;
}


void CLightDlg::OnEnKillfocusAngle(UINT _iID)
{
	int iID = _iID - 1029;
	m_bAngleFocus[iID % 2] = false;
	m_bChange = true;
}


void CLightDlg::OnEnChangeAngle(UINT _iID)
{
	int iID = _iID - 1029;
	if (false == m_bAngleFocus[iID % 2])
		return;

	CString str;
	m_arrAngleEdit[iID].GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (0.f == f)
		return;

	tLightInfo tInfo = m_pTargetObj->Light()->GetLightInfo();
	Vec2 vAngel = tInfo.vSpotAngle;
	if (0 == iID)
	{
		vAngel.x = cosf(XMConvertToRadians(f));
	}
	else
	{
		vAngel.y = cosf(XMConvertToRadians(f));
	}

	m_pTargetObj->Light()->SetAngle(vAngel);
}


void CLightDlg::OnCbnSelchange()
{
	tLightInfo tInfo = m_pTargetObj->Light()->GetLightInfo();
	tInfo.iLightType = m_cbType.GetCurSel();
	m_pTargetObj->Light()->SetLightInfo(tInfo);

	m_bChange = true;
}
