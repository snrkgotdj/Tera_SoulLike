// MeshRenderDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "MeshRenderDlg.h"
#include "afxdialogex.h"
#include "ListDlg.h"
#include "MaterialDlg.h"

#include "GameObject.h"
#include "MeshRender.h"
#include "Mesh.h"

// CMeshRenderDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMeshRenderDlg, CDialogEx)

CMeshRenderDlg::CMeshRenderDlg(CWnd* pParent /*=NULL*/)
	: CComponentDlg(IDD_MESHRENDERDLG, pParent)
	, m_pPrevMesh(NULL)
	, m_lStartPosY(0)
	, m_bShadowCast(FALSE)
	, m_bPostEffect(FALSE)
{

}

CMeshRenderDlg::~CMeshRenderDlg()
{
}

void CMeshRenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editMesh);
	DDX_Control(pDX, IDC_COMBO1, m_cbRS);
	DDX_Check(pDX, IDC_CHECK1, m_bShadowCast);
	DDX_Check(pDX, IDC_CHECK2, m_bPostEffect);
}


BOOL CMeshRenderDlg::OnInitDialog()
{
	CComponentDlg::OnInitDialog();

	m_cbRS.InsertString(0, L"RT_WIREFRAME");
	m_cbRS.InsertString(1, L"RT_FRONT");
	m_cbRS.InsertString(2, L"RT_BACK");
	m_cbRS.InsertString(3, L"RT_NONE");

	RECT rt = {};
	GetWindowRect(&rt);
	m_lStartPosY = rt.bottom - rt.top;

	return TRUE;
}

void CMeshRenderDlg::InitMaterial()
{
	Safe_Delete_Vec(m_vecMtrlDlg);
	m_vecMtrlDlg.clear();

	RECT rt = {};
	RECT rtMtrl = {};
	GetWindowRect(&rt);

	LONG lStartPosY = m_lStartPosY;

	CMeshRender* pMR = m_pTargetObj->MeshRender();
	UINT iSize = pMR->GetMaterialSize();

	CMaterialDlg* pMaterialDlg = NULL;
	for (UINT i = 0; i < iSize; ++i)
	{
		pMaterialDlg = new CMaterialDlg;
		pMaterialDlg->Create(IDD_MATERIALDLG, this);
		pMaterialDlg->ShowWindow(true);
		pMaterialDlg->UpdateWindow();

		pMaterialDlg->InitMaterial(pMR, i);

		pMaterialDlg->GetWindowRect(&rtMtrl);
		pMaterialDlg->SetWindowPos(NULL, 0, lStartPosY, rt.right - rt.left, rtMtrl.bottom - rtMtrl.top, 0);
		pMaterialDlg->GetWindowRect(&rtMtrl);
		lStartPosY += rtMtrl.bottom - rtMtrl.top - 2;
		m_vecMtrlDlg.push_back(pMaterialDlg);
	}

	if (0 == iSize)
		SetWindowPos(NULL, 0, 0, rt.right - rt.left, m_lStartPosY, SWP_NOMOVE);
	else
	{
		rt.bottom = rtMtrl.bottom;
		MoveWindow(&rt);
	}
}


void CMeshRenderDlg::UpdateComponent(CGameObject * _pObj)
{
	if (m_pTargetObj != _pObj)
	{
		m_editMesh.SetWindowTextW(L"");
	}

	m_pTargetObj = _pObj;

	if (NULL != m_pTargetObj)
	{
		for (UINT i = 0; i < m_vecMtrlDlg.size(); ++i)
		{
			m_vecMtrlDlg[i]->UpdateParam();
		}
	}

	if (false == m_bChange)
		return;;

	CMeshRender* pMR = m_pTargetObj->MeshRender();
	CString str;

	if (NULL != pMR->GetMesh())
	{
		str = pMR->GetMesh()->GetKey().c_str();
		m_editMesh.SetWindowTextW(str);
	}

	m_pPrevMesh = pMR->GetMesh();
	InitMaterial();

	int iSel = (UINT)pMR->GetRSType();
	m_cbRS.SetCurSel(iSel);

	m_bShadowCast = (pMR->IsShadowCast() != 0);
	m_bPostEffect = (pMR->IsPostEffect() != 0);
	UpdateData(false);

	m_bChange = false;
}


BEGIN_MESSAGE_MAP(CMeshRenderDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CMeshRenderDlg::OnBnClickedMesh)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMeshRenderDlg::OnCbnSelchangeRS)
	ON_BN_CLICKED(IDC_CHECK1, &CMeshRenderDlg::OnBnClickedShadow)
	ON_BN_CLICKED(IDC_CHECK2, &CMeshRenderDlg::OnBnClickedPostEffect)
	ON_BN_CLICKED(IDC_BUTTON1, &CMeshRenderDlg::OnBnClickedDelete)
END_MESSAGE_MAP()


// CMeshRenderDlg 메시지 처리기입니다.


void CMeshRenderDlg::OnBnClickedMesh()
{
	CListDlg dlg;

	dlg.InitList(RESOURCE_TYPE::MESH);

	CMesh* pMesh = (CMesh*)dlg.DoModal();

	if (NULL == pMesh)
		return;

	m_pTargetObj->MeshRender()->SetMesh(pMesh);

	m_bChange = true;
}

void CMeshRenderDlg::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CComponentDlg::PostNcDestroy();
}


void CMeshRenderDlg::OnCbnSelchangeRS()
{
	int iSel = m_cbRS.GetCurSel();
	m_pTargetObj->MeshRender()->SetRSMode((RASTERIZE_TYPE)iSel);
	::SetFocus(m_hWndOwner);
}


void CMeshRenderDlg::OnBnClickedShadow()
{
	if (m_bShadowCast)
	{
		m_bShadowCast = false;
	}
	else
	{
		m_bShadowCast = true;
	}

	UpdateData(false);

	if (NULL != m_pTargetObj)
		m_pTargetObj->MeshRender()->SetCastShadow(m_bShadowCast != 0);

	m_bChange = true;
}


void CMeshRenderDlg::OnBnClickedPostEffect()
{
	if (m_bPostEffect)
	{
		m_bPostEffect = false;
	}
	else
	{
		m_bPostEffect = true;
	}

	UpdateData(false);

	if (NULL != m_pTargetObj)
		m_pTargetObj->MeshRender()->SetPostEffect(m_bPostEffect != 0);

	m_bChange = true;
}


void CMeshRenderDlg::OnBnClickedDelete()
{
	m_pTargetObj->DeleteComponent(COMPONENT_TYPE::MESHRENDER);
}
