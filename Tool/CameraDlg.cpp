// CameraDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CameraDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "ListDlg.h"
#include "CamFilter.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Camera.h"

// CCameraDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CCameraDlg, CDialogEx)

CCameraDlg::CCameraDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_CAMERADLG, pParent)
	, m_btnPerspective(FALSE)
{

}

CCameraDlg::~CCameraDlg()
{
}

void CCameraDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_btnPerspective);
	DDX_Control(pDX, IDC_EDIT1, m_editWidth);
	DDX_Control(pDX, IDC_EDIT2, m_editHeght);
	DDX_Control(pDX, IDC_EDIT3, m_editScale);
	DDX_Control(pDX, IDC_EDIT4, m_editNear);
	DDX_Control(pDX, IDC_EDIT10, m_editFar);
	DDX_Control(pDX, IDC_EDIT11, m_editFilter);
	DDX_Control(pDX, IDC_COMBO2, m_cbLayerList);
	DDX_Control(pDX, IDC_LIST1, m_lbCheckLayer);
}

void CCameraDlg::UpdateComponent(CGameObject * _pObj)
{
	m_pTargetObj = _pObj;

	if (NULL == m_pTargetObj || !m_bChange)
		return;

	m_btnPerspective = (m_pTargetObj->Camera()->IsPerspective() != 0);
	UpdateData(false);

	CString str;
	CCamera* pCam = m_pTargetObj->Camera();

	if (m_btnPerspective)
	{
		m_editWidth.SetReadOnly();
		m_editHeght.SetReadOnly();
		m_editScale.SetReadOnly();

		str.Format(L"%.2f", pCam->GetNear());
		m_editNear.SetWindowTextW(str);
		str.Format(L"%.2f", pCam->GetFar());
		m_editFar.SetWindowTextW(str);	
	}
	else
	{
		m_editWidth.SetReadOnly(false);
		m_editHeght.SetReadOnly(false);
		m_editScale.SetReadOnly(false);

		str.Format(L"%.2f", pCam->GetWidth());
		m_editWidth.SetWindowTextW(str);
		str.Format(L"%.2f", pCam->GetHeght());
		m_editHeght.SetWindowTextW(str);
		str.Format(L"%.2f", pCam->GetScale());
		m_editScale.SetWindowTextW(str);
		str.Format(L"%.2f", pCam->GetNear());
		m_editNear.SetWindowTextW(str);
		str.Format(L"%.2f", pCam->GetFar());
		m_editFar.SetWindowTextW(str);
	}

	if (NULL == pCam->GetFilter())
		m_editFilter.SetWindowTextW(L"None");
	else
		m_editFilter.SetWindowTextW(pCam->GetFilter()->GetKey().c_str());

	m_cbLayerList.ResetContent();
	m_lbCheckLayer.ResetContent();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	vector<CLayer*> vecLayer = pCurScene->GetVecLayer();

	for (UINT i = 0; i < vecLayer.size(); ++i)
	{
		if (NULL == vecLayer[i])
			continue;

		m_cbLayerList.AddString(vecLayer[i]->GetLayerName().c_str());
	}

	for (UINT i = 0; i < vecLayer.size(); ++i)
	{
		if (NULL == vecLayer[i])
			continue;

		int iIdx = vecLayer[i]->GetLayerIdx();

		if (m_pTargetObj->Camera()->m_iLayerCheck & (1 << iIdx))
		{
			m_lbCheckLayer.AddString(vecLayer[i]->GetLayerName().c_str());
		}
	}

	m_bChange = false;
}

BEGIN_MESSAGE_MAP(CCameraDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK1, &CCameraDlg::OnBnClicked)
	ON_EN_CHANGE(IDC_EDIT1, &CCameraDlg::OnEnChangeWidth)
	ON_EN_CHANGE(IDC_EDIT2, &CCameraDlg::OnEnChangeHeight)
	ON_EN_CHANGE(IDC_EDIT3, &CCameraDlg::OnEnChangeScale)
	ON_EN_CHANGE(IDC_EDIT4, &CCameraDlg::OnEnChangeNear)
	ON_EN_CHANGE(IDC_EDIT10, &CCameraDlg::OnEnChangeFar)
	ON_BN_CLICKED(IDC_BUTTON2, &CCameraDlg::OnBnFilter)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CCameraDlg::OnCbnSelchange)
	ON_BN_CLICKED(IDC_BUTTON1, &CCameraDlg::OnBnRelease)
END_MESSAGE_MAP()


// CCameraDlg 메시지 처리기입니다.


void CCameraDlg::OnBnClicked()
{
	if (m_btnPerspective)
	{
		m_btnPerspective = false;
	}
	else
	{
		m_btnPerspective = true;
	}

	UpdateData(false);

	if (NULL != m_pTargetObj)
		m_pTargetObj->Camera()->SetPerspective(m_btnPerspective != 0);
	
	m_bChange = true;
}

void CCameraDlg::OnEnChangeWidth()
{
	CString str;
	m_editWidth.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (0 >= f)
		return;

	m_pTargetObj->Camera()->SetWidth(f);
}

void CCameraDlg::OnEnChangeHeight()
{
	CString str;
	m_editHeght.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (0 >= f)
		return;

	m_pTargetObj->Camera()->SetHeight(f);
}


void CCameraDlg::OnEnChangeScale()
{
	CString str;
	m_editScale.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (0 >= f)
		return;

	m_pTargetObj->Camera()->SetScale(f);
}

void CCameraDlg::OnEnChangeNear()
{
	CString str;
	m_editNear.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (m_pTargetObj->Camera()->GetFar() == f)
		return;

	m_pTargetObj->Camera()->SetNear(f);
}

void CCameraDlg::OnEnChangeFar()
{
	CString str;
	m_editFar.GetWindowTextW(str);
	float f = (float)_wtof(str.GetBuffer());

	if (m_pTargetObj->Camera()->GetNear() == f)
		return;

	m_pTargetObj->Camera()->SetFar(f);
}

void CCameraDlg::OnBnFilter()
{
	CListDlg dlg;
	dlg.InitList(RESOURCE_TYPE::Filter);
	CCamFilter* pFilter = (CCamFilter*)dlg.DoModal();
	m_pTargetObj->Camera()->SetFilter(pFilter);

	if (NULL == pFilter)
		m_editFilter.SetWindowTextW(L"None");
	else
		m_editFilter.SetWindowTextW(pFilter->GetKey().c_str());

	m_bChange = true;
}

void CCameraDlg::OnCbnSelchange()
{
	int iIdx = m_cbLayerList.GetCurSel();

	if (-1 == iIdx)
	{
		return;
	}

	CString str;
	m_cbLayerList.GetLBText(iIdx, str);

	m_pTargetObj->Camera()->LayerRenderCheck(str.GetBuffer());

	::SetFocus(m_hWndOwner);

	m_bChange = true;
}

void CCameraDlg::OnBnRelease()
{
	int iIdx = m_lbCheckLayer.GetCurSel();

	if (-1 == iIdx)
	{
		return;
	}

	CString str;
	m_lbCheckLayer.GetText(iIdx, str);

	m_pTargetObj->Camera()->LayerRenderCheck(str.GetBuffer());

	::SetFocus(m_hWndOwner);

	m_bChange = true;
}

void CCameraDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}
