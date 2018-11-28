#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "Transform.h"
#include "MeshRender.h"
#include "Camera.h"
#include "Collider_3D.h"
#include "Light.h"
#include "Trail.h"

#include "Mesh.h"
#include "MeshData.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "ResMgr.h"
#include "CollisionMgr.h"
#include "RenderMgr.h"
#include "Animator_3D.h"

#include "Material.h"

#include "GridScript.h"
#include "..\ScriptLib\MgrScript\GameSystemMgr.h"

#include "..\ScriptLib\CameraScript.h"
#include "..\ScriptLib\WarriorScript.h"
#include "..\ScriptLib\ThirdCamScript.h"

#include "../ScriptLib/MgrScript/GameSystemMgr.h"
#include "../ScriptLib/MgrScript/SkillMgr.h"
#include "../ScriptLib/MgrScript/EffectMgr.h"

CSceneMgr* CSceneMgr::pInst = nullptr;

CSceneMgr::CSceneMgr()
	: m_bUpdateEvent(false)
	, m_bPlayMod(false)
	, m_arrCamObj{}
	, m_pTerrainObj(NULL)
	, progress_func(NULL)
{
	SetPlayMod(false);
}

CSceneMgr::~CSceneMgr()
{
	SAFE_DELETE(m_pCurScene);
	SAFE_DELETE(m_pPrevScene);

	CSound::g_pSystem->release();
}


void CSceneMgr::SetPlayMod(bool _bTrue)
{
	m_bPlayMod = _bTrue;

	if (m_bPlayMod)
		progress_func = &CSceneMgr::ProgressPlay;
	else
		progress_func = &CSceneMgr::ProgressTool;
}

void CSceneMgr::PlayScene()
{
	SetPlayMod(true);

	if (m_bPlayMod && NULL != m_pCurScene)
	{
		m_pCurScene->Awake();
		m_pCurScene->Start();
	}
}

void CSceneMgr::SetCamera(const CAMERA_TYPE & _eType, CGameObject * _pCamObj)
{
	if (2 < (UINT)_eType)
		return;

	_pCamObj->Camera()->SetCameraType(_eType);
	m_arrCamObj[(UINT)_eType] = _pCamObj;
}

void CSceneMgr::Init()
{
	m_pCurScene = new CScene;
	 
	m_pCurScene->AddLayer(L"Default");
	m_pCurScene->AddLayer(L"Transparent");
	m_pCurScene->AddLayer(L"Camera");
	m_pCurScene->AddLayer(L"UI");
	m_pCurScene->AddLayer(L"Tool");
	m_pCurScene->AddLayer(L"Effect");
	m_pCurScene->AddLayer(L"Monster");
	m_pCurScene->AddLayer(L"Rock");
	m_pCurScene->AddLayer(L"Player");

	CGameObject* pObj = new CGameObject;
	pObj->SetTag(L"MainCamera");
	pObj->AddComponent<CTransform>(new CTransform);
	pObj->AddComponent<CCamera>(new CCamera);
	pObj->AddComponent<CScript>(new CCameraScript);

	pObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, -1.f));
	pObj->Camera()->LayerRenderCheck(L"Default");
	pObj->Camera()->LayerRenderCheck(L"Effect");
	pObj->Camera()->CreateSkyBox();

	CLayer* pCamLayer = m_pCurScene->FindLayer(L"Camera");
	pCamLayer->AddGameObject(pObj);

	SetCamera(CAMERA_TYPE::MAIN, pObj);
	
	//LoadFbx();
	//LoadMData();

	// 임시 테스트 신 구성하기
//#ifdef _DEBUG
	//CreateTestScene();
//#endif

	//CGameSystemMgr::GetInst()->Init();

	// Light 추가하기
	CGameObject* pLightObj = new CGameObject;
	pLightObj->SetTag(L"Light");
	pLightObj->AddComponent<CTransform>(new CTransform);
	pLightObj->AddComponent<CMeshRender>(new CMeshRender);
	pLightObj->AddComponent<CLight>(new CLight);

	pLightObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pLightObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	pLightObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pLightObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Default"));
	pLightObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	// LightInfo
	tLightCol tCol;
	tCol.vDiff = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	tCol.vSpec = Vec4(0.25f, 0.25f, 0.25f, 1.f);
	tCol.vAmb = Vec4(0.1f, 0.1f, 0.1f, 1.f);

	Vec3 vLightDir = Vec3(-1.f, -1.f, 1.f);
	vLightDir.Normalize();

	tLightInfo tInfo(tCol, Vec4(1000.f, 1000.f, -1000.f, 1.f)
		, vLightDir, (UINT)LIGHT_TYPE::DIR);

	pLightObj->Light()->SetLightInfo(tInfo);
	pLightObj->Light()->SetRange(200.f);
	//pLightObj->Light()->SetAngle(Vec2(cosf(XMConvertToRadians(65.f)), cosf(XMConvertToRadians(55.f))));
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pLightObj);
}

int CSceneMgr::Progress()
{
	(this->*progress_func)();

	return 0;
}

void CSceneMgr::Render()
{
	CRenderMgr::GetInst()->Render();
}

int CSceneMgr::ProgressPlay()
{
	ChangeFilter();

	m_pCurScene->Update();
	m_pCurScene->LateUpdate();
	m_pCurScene->FinalUpdate();

	CCollisionMgr::GetInst()->Update();

	CSound::Update();

	return 0;
}

int CSceneMgr::ProgressTool()
{
	m_pCurScene->DelayUpdate();
	m_pCurScene->FinalUpdate();

	CCollisionMgr::GetInst()->Update();

	return 0;
}

void CSceneMgr::ChangeFilter()
{
	if (KEY_INPUT(KEY_F8, STATE_TAB))
	{
		CCamFilter* pFilter = nullptr;
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);
	}

	else if (KEY_INPUT(KEY_F7, STATE_TAB))
	{
		CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_DepthOfField");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);
	}

	else if (KEY_INPUT(KEY_F6, STATE_TAB))
	{
		CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_Fog");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);
	}

	else if (KEY_INPUT(KEY_P, STATE_TAB))
	{
		CCamFilter* pFilter = nullptr;
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetHDRFilter(pFilter);
	}

	else if (KEY_INPUT(KEY_O, STATE_TAB))
	{
		CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_HDR");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetHDRFilter(pFilter);
	}
	else if (KEY_INPUT(KEY_I, STATE_TAB))
	{
		CEffectMgr::GetInst()->SetSoftParticle(true);
	}
	else if (KEY_INPUT(KEY_U, STATE_TAB))
	{
		CEffectMgr::GetInst()->SetSoftParticle(false);
	}
}

void CSceneMgr::ChangeScene(CScene * _pNextScene)
{
	if (NULL != m_pCurScene)
	{
		if (NULL != m_arrCamObj[(UINT)CAMERA_TYPE::TOOL])
		{
			m_arrCamObj[(UINT)CAMERA_TYPE::TOOL]->Camera()->Reset();
		}

		delete m_pCurScene;
	}
	
	m_pCurScene = _pNextScene;
}

void CSceneMgr::LoadFbx(const wstring& _strFileName)
{

	wstring strFolderPath = {};
	strFolderPath = CPathMgr::GetResPath();
	strFolderPath += L"Mesh\\" + _strFileName;

	WIN32_FIND_DATA tData = {};
	HANDLE hHandle = FindFirstFile(wstring(strFolderPath + L"\\*.*").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == hHandle)
	{
		MessageBox(NULL, L"존재하지 않는 폴더입니다", L"이펙트 로드 실패", MB_OK);
		assert(NULL);
	}

	wstring strExt;
	while (true)
	{
		strExt = CPathMgr::GetExt(tData.cFileName);
		if (L".fbx" == strExt || L".FBX" == strExt)
		{
			wstring strPath = L"Mesh\\MeshData\\";
			strPath += tData.cFileName;
			CMeshData* pMeshData = CResMgr::GetInst()->LoadFBX(strPath);
			pMeshData->SaveMeshData();

			INT a = 0;
		}

		if (0 == FindNextFile(hHandle, &tData))
			break;
	}
	FindClose(hHandle);
}

void CSceneMgr::LoadMData(const wstring & _strFileName)
{
	static bool g_bOnce = false;
	if (g_bOnce == true)
		return;
	g_bOnce = true;

	wstring strFolderPath = {};
	strFolderPath = CPathMgr::GetResPath();
	strFolderPath += L"Mesh\\" + _strFileName;

	WIN32_FIND_DATA tData = {};
	HANDLE hHandle = FindFirstFile(wstring(strFolderPath + L"\\*.*").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == hHandle)
	{
		MessageBox(NULL, L"존재하지 않는 폴더입니다", L"이펙트 로드 실패", MB_OK);
		assert(NULL);
	}

	wstring strExt;
	while (true)
	{
		strExt = CPathMgr::GetExt(tData.cFileName);
		if (L".mdat" == strExt )
		{
			wstring strPath = L"Mesh\\MeshData\\";
			strPath += tData.cFileName;
			CMeshData* pMeshData = CResMgr::GetInst()->LoadMeshData(strPath);
			CGameObject* pObj = pMeshData->Instantiate();
			pObj->SetTag(CPathMgr::GetFileName(tData.cFileName));
			CResMgr::GetInst()->AddPrefab(tData.cFileName, L"Default", pObj);
		}

		if (0 == FindNextFile(hHandle, &tData))
			break;
	}
	FindClose(hHandle);
}


void CSceneMgr::CreateTestScene()
{
	CLayer* pLayer = m_pCurScene->FindLayer(L"Default");
	//pLayer->SetZOrder(true);

	CreateGameObject();
}

#include "..\ScriptLib\MoveScript.h"
#include "..\ScriptLib\BillBoardScript.h"
#include "..\ScriptLib\LightMoveScript.h"
#include "..\ScriptLib\ArgoniteLeader.h"
#include "..\ScriptLib\BurnOutScript.h"
#include "..\ScriptLib\WaterScript.h"
#include "..\ScriptLib\AuraScript.h"
//#include "..\ScriptLib\DistortionEffect.h"

void CSceneMgr::CreateGameObject()
{
	//LoadFbx();
	//LoadMData();

	CMesh* pWaterMesh = CResMgr::GetInst()->CreatePlaneMesh(L"Water", 5, 5);
	CMaterial* pWaterMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Water");
	//CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Water_Difusse", L"Texture\\Water\\water.jpg");
	//pWaterMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTex);
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Water_Normal", L"Texture\\Water\\waterNormal.jpg");
	pWaterMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTex);
	
	pTex = CResMgr::GetInst()->FindTexture(L"Skybox_cube");
	pWaterMtrl->SetData(SHADER_PARAM::SP_TEXARR_7, &pTex);

	CGameObject* pWater = new CGameObject;

	pWater->SetTag(L"Water");
	pWater->AddComponent<CTransform>(new CTransform);
	pWater->AddComponent<CMeshRender>(new CMeshRender);
	pWater->AddComponent<CScript>(new CWaterScript);

	pWater->Transform()->SetLocalScale(Vec3(1000, 1, 1000));
	pWater->Transform()->SetLocalPos(Vec3(0, 100, 0));
	pWater->Transform()->SetCamCull(false);
	pWater->MeshRender()->SetMesh(pWaterMesh);
	pWater->MeshRender()->SetMaterial(pWaterMtrl);
	pWater->MeshRender()->SetPostEffect(true);

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pWater);


	/*CGameObject* pSword = new CGameObject;
	pSword->SetTag(L"Sword");
	pSword->AddComponent<CTransform>(new CTransform);
	pSword->AddComponent<CMeshRender>(new CMeshRender);
	pSword->AddComponent<CCollider>(new CCollider_3D);
	pSword->AddComponent<CTrail>(new CTrail);
	pSword->Trail()->SetPos(0, Vec3(0.5f, 0.f, 0.f));
	pSword->Trail()->SetPos(1, Vec3(-0.5f, 0.f, 0.f));
	pSword->Trail()->SetTrailMtrl(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_01", L"Texture\\Trail\\Trail_03.png"));
	pSword->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pSword->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->MeshRender()->SetPostEffect(true);

	pSword->Collider()->SetColType(COLLIDER_TYPE::OBB);
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pSword);*/

	//CGameObject* MotionTrail = new CGameObject;
	//MotionTrail->SetTag(L"MotionTrail");
	//MotionTrail->AddComponent<CTransform>(new CTransform);
	//MotionTrail->AddComponent<CMeshRender>(new CMeshRender);
	//MotionTrail->AddComponent<CTrail>(new CTrail);
	//MotionTrail->Trail()->SetPos(0, Vec3(0.5f, 0.f, 0.f));
	//MotionTrail->Trail()->SetPos(1, Vec3(-0.5f, 0.f, 0.f));
	//MotionTrail->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	//MotionTrail->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	//MotionTrail->MeshRender()->SetPostEffect(true);

	/*m_pCurScene->FindLayer(L"Default")->AddGameObject(MotionTrail);
	CGameObject* pClone1 = MotionTrail->Clone();
	CGameObject* pClone2 = MotionTrail->Clone();
	CGameObject* pClone3 = MotionTrail->Clone();

	m_pCurScene->FindLayer(L"Default")->AddGameObject(pClone1);
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pClone2);
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pClone3);*/


	/*CGameObject* pPlayer = CResMgr::GetInst()->FindPrefab(L"Player.mdat")->Instantiate();


	pPlayer->AddComponent<CScript>(new CArgoniteLeader);
	pPlayer->Animator_3D()->SetPartsObj(6, pSword);*/
	// 오오라 추가하기
	/*CAuraScript* pScript = new CAuraScript; 
	pPlayer->AddComponent<CScript>(pScript); pScript->init();

	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_Motion", L"Texture\\Trail\\Trail_White.jpg");
	MotionTrail->Trail()->SetTrailTexture(pTex);
	pClone1->Trail()->SetTrailTexture(pTex);
	pClone2->Trail()->SetTrailTexture(pTex);
	pClone3->Trail()->SetTrailTexture(pTex);

	
	pPlayer->SetTag(L"Player");
	pPlayer->Transform()->SetCamCull(false);
	pPlayer->Animator_3D()->SetPartsObj(46, pSword);
	pPlayer->Animator_3D()->SetPartsObj(5, MotionTrail);
	pPlayer->Animator_3D()->SetPartsObj(5, pClone1);
	pPlayer->Animator_3D()->SetPartsObj(5, pClone2);
	pPlayer->Animator_3D()->SetPartsObj(5, pClone3);
	m_pCurScene->FindLayer(L"Default")->AddGameObject(pPlayer);*/

	//CThirdCamScript* pCam = new CThirdCamScript;
	//pCam->SetThirdMod(pPlayer);
	//GetCamera(CAMERA_TYPE::MAIN)->AddScript(SCRIPT_ID::SCRIPT_CAMERA, new CCameraScript);

	/*pSword->Transform()->SetLocalPos(Vec3(50.f, 160.f, 5.f));
	pSword->Transform()->SetLocalRot(Vec3(0.f, 0.f, 0.0f));
	pSword->Transform()->SetLocalScale(Vec3(20.f, 20.f, 20.f));*/

	/*MotionTrail->Transform()->SetLocalPos(Vec3(0.f, 62.f, 2.f));
	MotionTrail->Transform()->SetLocalRot(Vec3(0.f, 0.0f, 1.40f));
	MotionTrail->Transform()->SetLocalScale(Vec3(20.f, 10.f, 10.f));

	pClone1->Transform()->SetLocalPos(Vec3(0.f, 60.f, -5.f));
	pClone1->Transform()->SetLocalRot(Vec3(0.f, 0.0f, 1.40f));
	pClone1->Transform()->SetLocalScale(Vec3(20.f, 10.f, 10.f));

	pClone2->Transform()->SetLocalPos(Vec3(0.f, 60.f, 5.f));
	pClone2->Transform()->SetLocalRot(Vec3(0.f, 0.0f, 1.40f));
	pClone2->Transform()->SetLocalScale(Vec3(20.f, 10.f, 10.f));

	pClone3->Transform()->SetLocalPos(Vec3(0.f, 58.f, -3.f));
	pClone3->Transform()->SetLocalRot(Vec3(0.f, 0.0f, 1.40f));
	pClone3->Transform()->SetLocalScale(Vec3(20.f, 20.f, 10.f));*/

	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pPlayer);


	// Light 추가하기
	//CGameObject* pLightObj = new CGameObject;
	//pLightObj->SetTag(L"Light");
	//pLightObj->AddComponent<CTransform>(new CTransform);
	//pLightObj->AddComponent<CMeshRender>(new CMeshRender);
	//pLightObj->AddComponent<CLight>(new CLight);

	//pLightObj->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//pLightObj->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));

	//pLightObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	//pLightObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Default"));
	//pLightObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	//CLightMoveScript* pLightScript = new CLightMoveScript;
	//pLightScript->SetFollowObj(pPlayer);

	//pLightObj->AddComponent<CScript>(pLightScript);

	//// LightInfo
	//tLightCol tCol;
	//tCol.vDiff = Vec4(1.f, 1.f, 1.f, 1.f);
	//tCol.vSpec = Vec4(0.4f, 0.4f, 0.4f, 1.f);
	//tCol.vAmb = Vec4(0.2f, 0.2f, 0.2f, 1.f);

	//Vec3 vLightDir = Vec3(-1.f, -1.f, 1.f);
	//vLightDir.Normalize();

	//tLightInfo tInfo(tCol, Vec4(1000.f, 1000.f, -1000.f, 1.f)
	//	, vLightDir, (UINT)LIGHT_TYPE::DIR);

	//pLightObj->Light()->SetLightInfo(tInfo);
	//pLightObj->Light()->SetRange(200.f);
	////pLightObj->Light()->SetAngle(Vec2(cosf(XMConvertToRadians(65.f)), cosf(XMConvertToRadians(55.f))));
	//m_pCurScene->FindLayer(L"Default")->AddGameObject(pLightObj);
}
