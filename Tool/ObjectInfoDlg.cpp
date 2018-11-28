// ObjectInfoDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Resource.h"
#include "ObjectInfoDlg.h"
#include "afxdialogex.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

// CObjectInfoDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectInfoDlg, CDialogEx)

CObjectInfoDlg::CObjectInfoDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_OBJECTINFODLG, pParent)
	, m_bActive(FALSE)
{

}

CObjectInfoDlg::~CObjectInfoDlg()
{
}

void CObjectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_bActive);
	DDX_Control(pDX, IDC_TAG, m_editObjName);
	DDX_Control(pDX, IDC_LAYER, m_cbLayer);
}

void CObjectInfoDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj || !m_bChange)
		return;

	if (false == m_arrFocus[0])
	{
		CString str;
		str = m_pTargetObj->GetTag().c_str(); 
		m_editObjName.SetWindowTextW(str);
	}

	if (false == m_arrFocus[1])
	{	
		m_cbLayer.ResetContent();

		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

		int iIdx = 0;
		for (UINT i = 0; i < vecLayer.size(); ++i)
		{
			if (NULL != vecLayer[i])
				m_cbLayer.InsertString(iIdx++, vecLayer[i]->GetLayerName().c_str());
		}

		iIdx = m_cbLayer.FindString(0, m_pTargetObj->GetLayerName().c_str());
		m_cbLayer.SetCurSel(iIdx);
	}


	m_bActive = (m_pTargetObj->IsActive() != 0);
	UpdateData(false);

	m_bChange = false;
}


BEGIN_MESSAGE_MAP(CObjectInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &CObjectInfoDlg::OnActiveBtn)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_TAG, IDC_LAYER, &CObjectInfoDlg::OnEnSetfocus)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_TAG, IDC_LAYER, &CObjectInfoDlg::OnEnKillfocus)
	ON_EN_CHANGE(IDC_TAG, &CObjectInfoDlg::OnEnChangeTag)
	ON_CBN_SELCHANGE(IDC_LAYER, &CObjectInfoDlg::OnCbnSelchangeLayer)
END_MESSAGE_MAP()


// CObjectInfoDlg 메시지 처리기입니다.

void CObjectInfoDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}

void CObjectInfoDlg::OnActiveBtn()
{
	if (m_bActive)
	{
		m_bActive = false;
	}
	else
	{
		m_bActive = true;
	}

	UpdateData(false);

	if (NULL != m_pTargetObj)
		m_pTargetObj->SetActive(m_bActive != 0);

	m_bChange = true;
}


void CObjectInfoDlg::OnEnSetfocus(UINT _id)
{
	UINT iID = _id - 1000;
	m_arrFocus[iID] = true;
}


void CObjectInfoDlg::OnEnKillfocus(UINT _id)
{
	UINT iID = _id - 1000;
	m_arrFocus[iID] = false;

#ifdef _TOOL
	CSceneMgr::GetInst()->SetUpdateCheck(true);
#endif
}

void CObjectInfoDlg::OnEnChangeTag()
{
	CString str;
	m_editObjName.GetWindowTextW(str);
	m_pTargetObj->SetTag(str.GetBuffer());

	m_bChange = true;
}


void CObjectInfoDlg::OnCbnSelchangeLayer()
{
	int iCurSel = m_cbLayer.GetCurSel();

	if (-1 == iCurSel)
	{
		return;
	}

	CString str;
	m_cbLayer.GetLBText(iCurSel, str);

	// 같은 레이어에 대한 예외 처리
	if (m_pTargetObj->GetLayerName() == str.GetBuffer())
		return;

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	pCurScene->FindLayer(str.GetBuffer())->AddGameObject(m_pTargetObj);

	::SetFocus(m_hWndOwner);

	m_bChange = true;
}