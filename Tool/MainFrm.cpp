
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrm.h"
#include "GameView.h"
#include "HierachyView.h"
#include "ComponentView.h"
#include "ButtonView.h"
#include "DebugView.h"
#include "AddMaterial.h"
#include "LayerSettingDlg.h"
#include "AddGameObjectDlg.h"

#include "Core.h"
#include "PathMgr.h"
#include "ResMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Camera.h"
#include "Terrain.h"
#include "Collider_3D.h"

#include "../External/SaveLoadMgr.h"
#include "ButtonScript.h"
#include "MgrScript\ScriptMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WORK_SPACE_X 1900
#define WORK_SPACE_Y 1070

#define GAME_VIEW_X 1200
#define GAME_VIEW_Y 675

#define BUTTON_VIEW_X GAME_VIEW_X
#define BUTTON_VIEW_Y 30

#define DEBUG_VIEW_X GAME_VIEW_X
#define DEBUG_VIEW_Y WORK_SPACE_Y - GAME_VIEW_Y - BUTTON_VIEW_Y

#define HIERACHY_VIEW_X 300
#define HIERACHY_VIEW_Y WORK_SPACE_Y

#define COMPONENT_VIEW_X WORK_SPACE_X - GAME_VIEW_X - HIERACHY_VIEW_X
#define COMPONENT_VIEW_Y WORK_SPACE_Y




// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SCENE_SAVE, &CMainFrame::OnSceneSave)
	ON_COMMAND(ID_SCENE_LOAD, &CMainFrame::OnSceneLoad)
	ON_COMMAND(ID_ADDRES_TEXTURE, &CMainFrame::OnAddresTexture)
	ON_COMMAND(ID_ADDRES_MATERIAL, &CMainFrame::OnAddresMaterial)
	ON_COMMAND(ID_GAMEOBJECT_CREATETERRAIN, &CMainFrame::OnGameobjectCreateterrain)
	ON_COMMAND(ID_UI_TEXT, &CMainFrame::OnCreateText)
	ON_COMMAND(ID_UI_BUTTON, &CMainFrame::OnCreateBotton)
	ON_COMMAND(ID_SCENE_LAYERSETTING, &CMainFrame::OnSceneLayersetting)
	ON_COMMAND(ID_GAMEOBJECT_SAVEGAMEOBJECT, &CMainFrame::OnLoadgameobject)
	ON_COMMAND(ID_EFFECT_CREATEBASEEFFECT, &CMainFrame::OnCreatebaseeffect)
	ON_COMMAND(ID_ADDGAMEOBJECT_BASIC, &CMainFrame::OnAddgameobjectBasic)
	ON_COMMAND(ID_ADDGAMEOBJECT_PREFAB, &CMainFrame::OnAddgameobjectPrefab)
	ON_COMMAND(ID_ADDGAMEOBJECT_WAYNODE, &CMainFrame::OnAddgameobjectWaynode)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
	: m_pAddGameObjectDlg(NULL)
	, m_bOnAddObjDlg(false)
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
	delete m_pAddGameObjectDlg;
}

void CMainFrame::Update()
{
	CComponentView* pComView = (CComponentView*)GetComponentView();
	pComView->Update();
	CButtonView* pBtnView = (CButtonView*)GetBtnView();
	pBtnView->Update();
}

void CMainFrame::SetOnAddObjDlg(bool _bTrue)
{
	m_bOnAddObjDlg = _bTrue;
	m_pAddGameObjectDlg->ShowWindow(_bTrue);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_MainSplitter.SetBorderSize( CPoint(0, 0), CPoint(1, 1));
	m_SubSplitter.SetBorderSize(CPoint(0, 0), CPoint(1, 1));

	// FrameWnd 3열 분할
	m_MainSplitter.CreateStatic(this, 1, 3);
	m_MainSplitter.SetColumnInfo(0, GAME_VIEW_X - 4, GAME_VIEW_X - 4);

	// Frame Wnd 0, 0 요소를 3행으로 분할
	m_SubSplitter.CreateStatic(&m_MainSplitter, 3, 1, WS_CHILD | WS_VISIBLE, m_MainSplitter.IdFromRowCol(0, 0));
	
	// 조각난 영역에 View 를 생성한다.
	m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CHierachyView), CSize(HIERACHY_VIEW_X, HIERACHY_VIEW_Y), NULL);
	m_MainSplitter.CreateView(0, 2, RUNTIME_CLASS(CComponentView), CSize(COMPONENT_VIEW_X, COMPONENT_VIEW_Y), NULL);

	m_SubSplitter.CreateView(1, 0, RUNTIME_CLASS(CGameView), CSize(GAME_VIEW_X, GAME_VIEW_Y), NULL);

	// Core 초기화
	if (FAILED(CCore::GetInst()->init(GetGameViewHwnd(), true)))
	{
		return FALSE;
	}

	m_SubSplitter.CreateView(0, 0, RUNTIME_CLASS(CButtonView), CSize(BUTTON_VIEW_X, BUTTON_VIEW_Y), NULL);	
	m_SubSplitter.CreateView(2, 0, RUNTIME_CLASS(CDebugView), CSize(DEBUG_VIEW_X, DEBUG_VIEW_Y), NULL);

	m_pAddGameObjectDlg = new CAddGameObjectDlg;
	m_pAddGameObjectDlg->Create(IDD_ADDGAMEOBJECTDLG, this);
	m_pAddGameObjectDlg->ShowWindow(false);
	m_pAddGameObjectDlg->UpdateWindow();
	SetOnAddObjDlg(false);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);


	RECT rt = { 0, 0, WORK_SPACE_X, WORK_SPACE_Y };

	AdjustWindowRect(&rt, cs.style, true);

	cs.cx = rt.right - rt.left + 2;
	cs.cy = rt.bottom - rt.top + 2;

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{	
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 그렇지 않으면 기본 처리합니다.
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

#include "TerrainScript.h"

void CMainFrame::OnGameobjectCreateterrain()
{
	UINT iFaceX = 64;
	UINT iFaceZ = 64;

	CMesh* pMesh = CResMgr::GetInst()->CreatePlaneMesh(L"TerrainMesh", iFaceX, iFaceZ, D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"Default");

	CGameObject* pObj = new CGameObject;

	pObj->SetTag(L"Terrain");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->AddComponent<CMeshRender>(new CMeshRender);
	pObj->AddComponent<CTerrain>(new CTerrain);

	pObj->Transform()->SetCamCull(false);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

	pObj->MeshRender()->SetMesh(pMesh);
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Terrain"));

	pObj->Terrain()->SetFaceCount(iFaceX, iFaceZ);

	CTexture* MapTex = CResMgr::GetInst()->CreateTexture(L"HEIGHT_MAP", iFaceX * 16, iFaceZ * 16, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32_FLOAT);
	pObj->Terrain()->SetHeightMap(MapTex);

	MapTex = CResMgr::GetInst()->CreateTexture(L"WEIGHT_TEX", iFaceX * 16, iFaceZ * 16, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM);
	pObj->Terrain()->SetWeightMap(MapTex);

	pObj->Terrain()->Init();
	pLayer->AddGameObject(pObj);

	CComponentView* pView = (CComponentView*)GetComponentView();
	pView->SetTarget(pObj);

	CSceneMgr::GetInst()->SetTerrainObject(pObj);
}

#include "Text.h"
void CMainFrame::OnCreateText()
{
	CGameObject* pObj = new CGameObject;

	pObj->SetTag(L"Text");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->AddComponent<CText>(new CText);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"UI");
	pLayer->AddGameObject(pObj);

	pCurScene->GetUIObject()->AddChildObj(pObj);
	CComponentView* pView = (CComponentView*)GetComponentView();
	pView->SetTarget(pObj);
}

void CMainFrame::OnCreateBotton()
{
	CGameObject* pObj = new CGameObject;

	pObj->SetTag(L"Button");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->AddComponent<CMeshRender>(new CMeshRender);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

	CMesh* pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh");
	pObj->MeshRender()->SetMesh(pMesh);
	pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"ButtonMtrl"));

	CButtonScript* pScript = new CButtonScript();
	pObj->AddComponent<CScript>(pScript);
	pScript->Init();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"UI");
	pLayer->AddGameObject(pObj);

	pCurScene->GetUIObject()->AddChildObj(pObj);

	CComponentView* pView = (CComponentView*)GetComponentView();
	pView->SetTarget(pObj);
}

void CMainFrame::OnSceneSave()
{
	wchar_t szFilter[] = L"Scene (*.scene) | *.scene; |";

	CFileDialog dlg(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT, szFilter);
	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Scene";
	dlg.m_ofn.lpstrInitialDir = strInitPath;
	
	CString strPathName;
	if (IDOK == dlg.DoModal()) {

		strPathName = dlg.GetPathName();
		wchar_t* pExt = CPathMgr::GetExt(strPathName.GetBuffer());

		if (0 == wcslen(pExt))
			strPathName += L".scene";

		CSaveLoadMgr::GetInst()->SaveScene(strPathName.GetBuffer());
	}	
}

void CMainFrame::OnSceneLoad()
{
	wchar_t szFilter[] = L"Scene (*.scene) | *.scene; |";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY, szFilter);
	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Scene";
	dlg.m_ofn.lpstrInitialDir = strInitPath;

	CString strPathName;
	if (IDOK == dlg.DoModal()) {

		// ComponentView 초기화
		((CHierachyView*)GetHierachyView())->Init();
		((CComponentView*)GetComponentView())->InitTarget();

		strPathName = dlg.GetPathName();
		CSaveLoadMgr::GetInst()->LoadScene(strPathName.GetBuffer());		
	}
}

void CMainFrame::OnAddresTexture()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_READONLY);
	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Texture";
	dlg.m_ofn.lpstrInitialDir = strInitPath;

	wstring strFilePath;
	if (IDOK == dlg.DoModal())
	{
		// ComponentView 초기화
		((CHierachyView*)GetHierachyView())->Init(TREECTRLDLG::RES);
		//((CComponentView*)GetComponentView())->InitTarget();

		strFilePath = dlg.GetPathName();
		UINT iStrIdx = strFilePath.find(L"Texture");
		strFilePath.erase(0, iStrIdx - 1);

		CString strName = strFilePath.c_str();
		(CTexture*)CResMgr::GetInst()->Load<CTexture>(CPathMgr::GetFileName(strName.GetBuffer()), strFilePath);
	}
}


void CMainFrame::OnAddresMaterial()
{
	CAddMaterial dlg;
	dlg.DoModal();
}

void CMainFrame::OnSceneLayersetting()
{
	CLayerSettingDlg dlg;
	dlg.DoModal();

	// ComponentView 초기화
	((CHierachyView*)GetHierachyView())->Init(TREECTRLDLG::HIERARCHY);
	((CComponentView*)GetComponentView())->InitTarget();
}

void CMainFrame::OnLoadgameobject()
{
	wchar_t szFilter[] = L"Scene File (*.goc) |*.goc;|";

	CFileDialog dlg(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT, szFilter);
	CString strInitPath = CPathMgr::GetResPath();
	strInitPath += L"Save\\GameObject";
	dlg.m_ofn.lpstrInitialDir = strInitPath;
	CGameObject* pObj = nullptr;
	if (IDOK == dlg.DoModal())
	{
		CString strPathName = dlg.GetPathName();
		pObj = CSaveLoadMgr::GetInst()->LoadGameObject(strPathName.GetBuffer());
	}
}


void CMainFrame::OnCreatebaseeffect()
{
	/*CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"Effect");

	CGameObject* pObj = new CGameObject;
	pObj->SetTag(L"BaseEffect");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->AddComponent<CMeshRender>(new CMeshRender);
	pObj->AddComponent<CAnimator_2D>(new CAnimator_2D);

	CScript* pScript = CScriptMgr::GetScript(L"BillBoardScript");
	pObj->AddScript(SCRIPT_ID::SCRIPT_BILLBOARD, pScript);
	pScript = CScriptMgr::GetScript(L"EffectScript");
	pObj->AddScript(SCRIPT_ID::SCRIPT_EFFECTSCRIPT, pScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

	CMesh* pRectMesh = CResMgr::GetInst()->FindMesh(L"RectMesh");
	CMaterial* pMaterial = CResMgr::GetInst()->FindMaterial(L"Mtrl_BlackEffect");

	pObj->MeshRender()->SetMesh(pRectMesh);
	pObj->MeshRender()->SetMaterial(pMaterial);
	pObj->MeshRender()->SetPostEffect(true);

	pLayer->AddGameObject(pObj);

	CComponentView* pView = (CComponentView*)GetComponentView();
	pView->SetTarget(pObj);*/
}

void CMainFrame::OnAddgameobjectBasic()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"Default");

	CGameObject* pObj = new CGameObject;
	pObj->SetTag(L"EmptyObject");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	pLayer->AddGameObject(pObj);

	CComponentView* pView = (CComponentView*)GetComponentView();
	pView->SetTarget(pObj);
}

#include "AddGameObjectDlg.h"
void CMainFrame::OnAddgameobjectPrefab()
{
	SetOnAddObjDlg(true);
}

void CMainFrame::OnAddgameobjectWaynode()
{
	CGameObject* pWayNode = new CGameObject;

	pWayNode->SetTag(L"WayNode");
	pWayNode->AddComponent<CTransform>(new CTransform);
	pWayNode->AddComponent<CMeshRender>(new CMeshRender);
	pWayNode->AddComponent<CCollider>(new CCollider_3D);

	pWayNode->Transform()->SetLocalPos(Vec3(0.f, 100.f, 0.f));
	pWayNode->Transform()->SetLocalScale(Vec3(10.f, 10.f, 10.f));
	pWayNode->Transform()->SetLocalRot(Vec3(0.f, 90.f, 1.f));
	pWayNode->Transform()->SetCamCull(false);
	pWayNode->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
	pWayNode->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Std3D"));

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(pWayNode);
}