// ButtonView.cpp : implementation file
//

#include "stdafx.h"
#include "Tool.h"
#include "ButtonView.h"


#include "SceneMgr.h"
#include "MainFrm.h"
#include "ComponentView.h"
#include "HierachyView.h"
#include "PathMgr.h"
#include "CameraScript.h"
#include "SaveLoadMgr.h"

#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GridScript.h"
#include "RenderMgr.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "ToolUIScript.h"

// CButtonView

IMPLEMENT_DYNCREATE(CButtonView, CFormView)

CButtonView::CButtonView()
	: CFormView(IDD_BUTTONVIEW)
	, m_bPlay(false)
	, m_pCamObj(NULL)
	, m_pGridObj(NULL)
{

}

CButtonView::~CButtonView()
{
	SAFE_DELETE(m_pCamObj);
	SAFE_DELETE(m_pGridObj);	
	//SAFE_DELETE(m_pFPSObj);

	Safe_Delete_List(m_listRTObj);
}

void CButtonView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON2, m_btnPlay);
	DDX_Control(pDX, IDC_BUTTON3, m_btnPause);
	DDX_Control(pDX, IDC_BUTTON1, m_btnStop);

	m_btnPlay.EnableWindow(true);
	m_btnPause.EnableWindow(false);
	m_btnStop.EnableWindow(false);
}

BEGIN_MESSAGE_MAP(CButtonView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON2, &CButtonView::OnPlayBtn)
	ON_BN_CLICKED(IDC_BUTTON3, &CButtonView::OnPauseBtn)
	ON_BN_CLICKED(IDC_BUTTON1, &CButtonView::OnStopBtn)
	ON_WM_CREATE()
END_MESSAGE_MAP()

void CButtonView::Update()
{
	if (m_bPlay)
		return;

	m_pCamObj->Update();
	m_pGridObj->Update();
	for (auto& pObj : m_listRTObj)
		pObj->Update();

	m_pCamObj->LateUpdate();
	m_pGridObj->LateUpdate();
	for (auto& pObj : m_listRTObj)
		pObj->LateUpdate();

	m_pCamObj->FinalUpdate();
	m_pGridObj->FinalUpdate();
	for (auto& pObj : m_listRTObj)
		pObj->FinalUpdate();

	//if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_0, STATE_TAB))
	//{
	//	CMesh* pWaterMesh = CResMgr::GetInst()->CreatePlaneMesh(L"Water", 5, 5);
	//	CMaterial* pWaterMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Water");
	//	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Water_Difusse", L"Texture\\Water\\water.jpg");
	//	//pWaterMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTex);
	//	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Water_Normal", L"Texture\\Water\\waterNormal.jpg");
	//	pWaterMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTex);

	//	pTex = CResMgr::GetInst()->FindTexture(L"Skybox_cube");
	//	pWaterMtrl->SetData(SHADER_PARAM::SP_TEXARR_7, &pTex);

	//	CGameObject* pWater = new CGameObject;

	//	pWater->SetTag(L"Water");
	//	pWater->AddComponent<CTransform>(new CTransform);
	//	pWater->AddComponent<CMeshRender>(new CMeshRender);

	//	pWater->Transform()->SetLocalScale(Vec3(1000, 1, 1000));
	//	pWater->Transform()->SetLocalPos(Vec3(0, 100, 0));
	//	pWater->Transform()->SetCamCull(false);
	//	pWater->MeshRender()->SetMesh(pWaterMesh);
	//	pWater->MeshRender()->SetMaterial(pWaterMtrl);
	//	pWater->MeshRender()->SetPostEffect(true);

	//	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(pWater);
	//}

}

void CButtonView::CreateToolObj()
{
	// Tool 전용 카메라 생성
	m_pCamObj = new CGameObject;

	m_pCamObj->SetTag(L"ToolCamera");
	m_pCamObj->AddComponent<CTransform>(new CTransform);
	m_pCamObj->AddComponent<CCamera>(new CCamera);
	m_pCamObj->AddComponent<CScript>(new CCameraScript);

	m_pCamObj->Camera()->SetAllLayerCheck();
	m_pCamObj->Camera()->CreateSkyBox();
	//m_pCamObj->Camera()->SetFilter(CRenderMgr::GetInst()->FindFilter(L"DistortionFilter"));

	m_pCamObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, -1.f));
	CSceneMgr::GetInst()->SetCamera(CAMERA_TYPE::TOOL ,m_pCamObj);

	m_pCamObj->Awake();
	m_pCamObj->Start();

	// Grid Object 만들기
	m_pGridObj = new CGameObject;
	m_pGridObj->SetTag(L"Grid");
	//m_pGridObj->AddComponent<CTransform>(new CTransform);
	//m_pGridObj->AddComponent<CMeshRender>(new CMeshRender);
	//m_pGridObj->AddComponent<CScript>(new CGridScript);

	//m_pGridObj->Transform()->SetLocalScale(Vec3(100000.f, 100000.f, 100000.f));
	//m_pGridObj->Transform()->SetLocalRot(Vec3(XM_PI / 2.f, 0.f, 0.f));

	//m_pGridObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	//m_pGridObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Grid"));
	//m_pGridObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	m_pCamObj->Camera()->AddToolObj(m_pGridObj);

	// RenderTarget UI
	CGameObject* pUIObj = NULL;

	for (UINT i = 0; i < 11; ++i)
	{
		pUIObj = new CGameObject;

		wchar_t szName[50] = {};
		wsprintf(szName, L"Target UI %d", i);
		pUIObj->SetTag(szName);
		pUIObj->AddComponent<CTransform>(new CTransform);
		pUIObj->AddComponent<CMeshRender>(new CMeshRender);

		pUIObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
		pUIObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_ToolUI"));

		CToolUIScript* pScript = new CToolUIScript;
		pUIObj->AddComponent<CScript>(pScript);
		pScript->SetUINumber(i);

		pUIObj->Start();
		m_listRTObj.push_back(pUIObj);
		m_pCamObj->Camera()->AddToolObj(pUIObj);
	}
}


// CButtonView diagnostics

#ifdef _DEBUG
void CButtonView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CButtonView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CButtonView message handlers
void CButtonView::OnPlayBtn()
{
	m_btnPlay.EnableWindow(false);
	m_btnPause.EnableWindow(true);
	m_btnStop.EnableWindow(true);

	if (m_bPlay)
	{
		CSceneMgr::GetInst()->SetPlayMod(true);
		return;
	}	
	else if (CSceneMgr::GetInst()->IsPlayMode())
		return;	

	// Scene 의 현재 상태를 임시로 저장해둔다.
	wstring strPath = CPathMgr::GetResPath();
	strPath += L"Temp.scene";
	CSaveLoadMgr::GetInst()->SaveScene(strPath);

	CSceneMgr::GetInst()->PlayScene();

	m_bPlay = true;
}

void CButtonView::OnPauseBtn()
{
	m_bPlay = false;

	m_btnPlay.EnableWindow(true);
	m_btnPause.EnableWindow(false);
	m_btnStop.EnableWindow(true);

	CSceneMgr::GetInst()->SetPlayMod(false);
}

void CButtonView::OnStopBtn()
{
	m_bPlay = false;	
	CSceneMgr::GetInst()->SetPlayMod(false);
	
	// 재생 시작 당시의 Save 파일로 돌아간다.
	// ComponentView 초기화
	CMainFrame* pMainFrm = (CMainFrame*)AfxGetMainWnd();
	((CHierachyView*)pMainFrm->GetHierachyView())->Init();
	((CComponentView*)pMainFrm->GetComponentView())->InitTarget();

	wstring strPath = CPathMgr::GetResPath();
	strPath += L"Temp.scene";
	CSaveLoadMgr::GetInst()->LoadScene(strPath);

	// Tool Camera 초기화
	m_pCamObj->Camera()->Reset();

	m_btnPlay.EnableWindow(true);
	m_btnPause.EnableWindow(false);
	m_btnStop.EnableWindow(false);

}

int CButtonView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Tool 전용 쉐이더 및 메테리얼
	CreateToolObj();

	return 0;
}
