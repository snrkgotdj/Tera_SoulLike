#include "EffectMgr.h"
#include "SceneMgr.h"
#include "SaveLoadMgr.h"
#include "PathMgr.h"
#include "ResMgr.h"

#include "Animator_2D.h"
#include "Animation_2D.h"

#include "Layer.h"
#include "Scene.h"
#include "Script.h"

#include "../BillBoardScript.h"
#include "../ElectricEffect.h"
#include "../BloodEffect.h"
#include "../TileEffect.h"

CEffectMgr::CEffectMgr()
	: m_iCallCnt(0)
	, m_bSoftParticle(true)
{
	m_ID = SCRIPT_EFFECTMGR;
	//Init();
}

CEffectMgr::~CEffectMgr()
{
}

void CEffectMgr::ActiveEffect(const wstring& _strEffect, const Vec3& _vecPos)
{
	vector<CGameObject*>& vecEffect = FindEffect(_strEffect);

	if (vecEffect.size() == 0)
	{
		MessageBox(nullptr, L"해당 이름의 이펙트가 없습니다.", L"이펙트 실행 오류", MB_OK);
		return;
	}

	CGameObject* pEffectObj = NULL;
	for (UINT i = 0; i < vecEffect.size(); ++i)
	{
		if (false == vecEffect[i]->IsActive())
		{
			vecEffect[i]->SetActive(true);
			pEffectObj = vecEffect[i];
			break;
		}
	}

	if (pEffectObj == NULL)
	{
		CGameObject* pClone = vecEffect[0]->Clone();
		pClone->MeshRender()->GetMaterial();

		if (pClone->HasParent())
			pClone->ClearParent();
		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect")->AddGameObject(pClone);

		vecEffect.push_back(pClone);
		pEffectObj = pClone;
	}

	pEffectObj->SetActive(true);
	pEffectObj->Animator_2D()->GetCurAnim()->SetActive(true);
	pEffectObj->Animator_2D()->GetCurAnim()->SetFrameIdx(0);
	pEffectObj->Transform()->SetLocalPos(_vecPos);
}

CGameObject* CEffectMgr::ActiveEffect(const wstring & _strEffect, const Vec3 & _vecPos, const Vec3 & _vScale, float _fTerm, bool _bSoftParticle)
{
	vector<CGameObject*>& vecEffect = FindEffect(_strEffect);

	if (vecEffect.size() == 0)
	{
		MessageBox(nullptr, L"해당 이름의 이펙트가 없습니다.", L"이펙트 실행 오류", MB_OK);
		return nullptr;
	}

	CGameObject* pEffectObj = NULL;
	for (UINT i = 0; i < vecEffect.size(); ++i)
	{
		if (false == vecEffect[i]->IsActive())
		{
			vecEffect[i]->SetActive(true);
			pEffectObj = vecEffect[i];
			break;
		}
	}

	if (pEffectObj == NULL)
	{
		CGameObject* pClone = vecEffect[0]->Clone();

		if (pClone->HasParent())
			pClone->ClearParent();
		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect")->AddGameObject(pClone);

		vecEffect.push_back(pClone);
		pEffectObj = pClone;
	}

	pEffectObj->SetActive(true);
	pEffectObj->Animator_2D()->GetCurAnim()->SetActive(true);
	pEffectObj->Animator_2D()->GetCurAnim()->SetFrameIdx(0);
	pEffectObj->Transform()->SetLocalPos(_vecPos);
	pEffectObj->Transform()->SetLocalScale(_vScale);

	if (_fTerm > 0.000001)
	{
		((CAnimation_2D*)pEffectObj->Animator_2D()->GetCurAnim())->SetTerm(_fTerm);
	}

	if (_bSoftParticle && m_bSoftParticle)
	{
		CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_SoftParticle");
		pEffectObj->MeshRender()->SetMaterial(pMtrl);
	}

	else
	{
		CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
		pEffectObj->MeshRender()->SetMaterial(pMtrl);
	}

	return pEffectObj;
}

vector<CGameObject*>& CEffectMgr::FindEffect(const wstring& _strEffect)
{
	map<wstring, vector<CGameObject*>>::iterator iter = m_mapEffect.find(_strEffect);

	if (iter == m_mapEffect.end())
	{
		vector<CGameObject*> vecEmpty;
		return vecEmpty;
	}

	return iter->second;
}

void CEffectMgr::Awake()
{
	
}

int CEffectMgr::Update()
{

	return 0;
}

void CEffectMgr::Init()
{
	CGameObject* pEffect = new CGameObject;
	pEffect->SetTag(L"Electric_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddScript(SCRIPT_ID::SCRIPT_ELECTRIC_EFFECT, new CElectricEffect);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
	pEffect->MeshRender()->SetPostEffect(true);
	CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Electric_Effect");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Electric", L"Texture\\Effect\\Electric.jpg");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTex);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Effect_Electric", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::ELECTRIC] = L"Effect_Electric";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Electric_Cast_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalScale(Vec3(80.f, 80.f, 80.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);

	tDescInfo info = {};
	info.strTexName = L"ElectricCast.png";
	info.strAnimName = L"ElectricCast";
	info.iContinue = 0;
	info.iFrameCountX = 6;
	info.iFrameCountY = 3;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 380.f;
	info.vSize.y = 380.f;

	wstring strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	pEffect->Animator_2D()->SetTerm(L"ElectricCast", 0.1f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Electric_Cast_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::ELECTRIC_CAST] = L"Electric_Cast_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Electric_End_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalScale(Vec3(50.f, 50.f, 50.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);

	info = {};
	info.strTexName = L"ElectricBoom.jpg";
	info.strAnimName = L"ElectricBoom";
	info.iContinue = 0;
	info.iFrameCountX = 4;
	info.iFrameCountY = 4;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 160.f;
	info.vSize.y = 160.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	//pEffect->Animator_2D()->SetTerm(L"ElectricBoom", 0.05f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Electric_End_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::ELECTRIC_END] = L"Electric_End_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Blood_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);
	pEffect->AddScript(SCRIPT_Blood_EFFECT, new CBloodEffect);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D");
	pEffect->MeshRender()->SetMaterial(pMtrl);

	info = {};
	info.strTexName = L"Blood.png";
	info.strAnimName = L"Blood";
	info.iContinue = 0;
	info.iFrameCountX = 4;
	info.iFrameCountY = 4;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 128.f;
	info.vSize.y = 128.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	//pEffect->Animator_2D()->SetTerm(L"Blood", 0.9f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Blood_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::BLOOD] = L"Blood_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Blood_Effect_Tile");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	CTileEffect* pTile = new CTileEffect;
	pEffect->AddScript(SCRIPT_TILE, pTile);
	pTile->SetTime(8.f);

	pEffect->Transform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Tile_Effect");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetPostEffect(true);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Blood_Effect_Tile", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::BLOOD_TILE] = L"Blood_Effect_Tile";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Wave_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Wave_Effect");
	pEffect->MeshRender()->SetMaterial(pMtrl);	
	pEffect->MeshRender()->SetPostEffect(true);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Wave_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::WAVE] = L"Wave_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Dust_Ani_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalScale(Vec3(1000.f, 1000.f, 1000.f));
	pEffect->Transform()->SetLocalRot(Vec3(1.57f, 0.f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);

	info = {};
	info.strTexName = L"Dust_Ani.png";
	info.strAnimName = L"Dust_Ani";
	info.iContinue = 0;
	info.iFrameCountX = 4;
	info.iFrameCountY = 4;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 256.f;
	info.vSize.y = 256.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Dust_Ani_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::DUST_ANI] = L"Dust_Ani_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Dust_Ani2_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalScale(Vec3(1000.f, 1000.f, 1000.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);

	info = {};
	info.strTexName = L"Dust2_Ani.png";
	info.strAnimName = L"Dust2_Ani";
	info.iContinue = 0;
	info.iFrameCountX = 5;
	info.iFrameCountY = 20;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 500.f;
	info.vSize.y = 280.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Dust_Ani2_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::DUST2_ANI] = L"Dust_Ani2_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Potal_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalRot(Vec3(0.f, 1.54f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Common Effects_1016.png";
	info.strAnimName = L"Potal";
	info.iContinue = 1;
	info.iFrameCountX = 44;
	info.iFrameCountY = 0;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 976.f;
	info.vSize.y = 686.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect\\Potal";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Potal_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::POTAL] = L"Potal_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Potal_Electiric_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);

	pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pEffect->Transform()->SetLocalRot(Vec3(0.f, 1.54f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Potal_Electiric.png";
	info.strAnimName = L"Potal_Electiric";
	info.iContinue = 0;
	info.iFrameCountX = 5;
	info.iFrameCountY = 11;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 800.f;
	info.vSize.y = 450.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Potal_Electiric_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::POTAL_Electric] = L"Potal_Electiric_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Smoke1_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalScale(Vec3(200.f, 200.f, 200.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Smoke1.jpg";
	info.strAnimName = L"Smoke1";
	info.iContinue = 0;
	info.iFrameCountX = 4;
	info.iFrameCountY = 4;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 300.f;
	info.vSize.y = 300.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	pEffect->Animator_2D()->SetTerm(L"Smoke1", 0.09f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Smoke1", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::SMOKE1] = L"Smoke1";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Crack_Effect_Tile");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pTile = new CTileEffect;
	pEffect->AddScript(SCRIPT_TILE, pTile);
	pTile->SetTime(12.f);

	pEffect->Transform()->SetLocalScale(Vec3(300.f, 300.f, 300.f));
	pEffect->Transform()->SetLocalRot(Vec3(1.57f, 0.f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Tile_Effect");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetPostEffect(true);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Crack_Effect_Tile", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::CRACK_TILE] = L"Crack_Effect_Tile";

	//pEffect = new CGameObject;
	//pEffect->SetTag(L"Spwan_Effect");
	//pEffect->AddComponent<CTransform>(new CTransform);
	//pEffect->AddComponent<CMeshRender>(new CMeshRender);
	//pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);

	//pEffect->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	//pEffect->Transform()->SetLocalRot(Vec3(0.f, 1.54f, 0.f));

	//pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	//pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	//pEffect->MeshRender()->SetMaterial(pMtrl);
	//pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	//info = {};
	//info.strTexName = L"Common Effects_2188.png";
	//info.strAnimName = L"Spwan";
	//info.iContinue = 1;
	//info.iFrameCountX = 13;
	//info.iFrameCountY = 0;
	//info.vLeftTop.x = 0.f;
	//info.vLeftTop.y = 0.f;
	//info.vSize.x = 976.f;
	//info.vSize.y = 686.f;

	//strPath = CPathMgr::GetResPath();
	//strPath += L"Texture\\Effect\\Spwan";

	//pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);

	//if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Spwan_Effect", L"Default", pEffect))
	//	delete pEffect;

	//m_strKey[(UINT)EFFECT_TYPE::SPWAN] = L"Spwan_Effect";

	CResMgr::GetInst()->Load<CTexture>(L"Blood_1", L"Texture\\Effect\\Blood_1.png");
	CResMgr::GetInst()->Load<CTexture>(L"Crack", L"Texture\\Effect\\Crack.png");
	CResMgr::GetInst()->Load<CTexture>(L"Crack2", L"Texture\\Effect\\Crack2.png");

	pEffect = new CGameObject;
	pEffect->SetTag(L"Flare0_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalScale(Vec3(4500.f, 3000.f, 4500.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Flare_0.png";
	info.strAnimName = L"Flare_0";
	info.iContinue = 0;
	info.iFrameCountX = 5;
	info.iFrameCountY = 6;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 612.f;
	info.vSize.y = 344.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	pEffect->Animator_2D()->SetTerm(L"Flare_0", 0.025f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Flare0_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::FLARE_0] = L"Flare0_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Flare1_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	//pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalScale(Vec3(3000.f, 3000.f, 1.f));
	pEffect->Transform()->SetLocalRot(Vec3(1.57f, 0.f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Flare_1.png";
	info.strAnimName = L"Flare_1";
	info.iContinue = 0;
	info.iFrameCountX = 5;
	info.iFrameCountY = 5;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 450.f;
	info.vSize.y = 450.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	pEffect->Animator_2D()->SetTerm(L"Flare_1", 0.01f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Flare1_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::FLARE_1] = L"Flare1_Effect";

	pEffect = new CGameObject;
	pEffect->SetTag(L"Flare2_Effect");
	pEffect->AddComponent<CTransform>(new CTransform);
	pEffect->AddComponent<CMeshRender>(new CMeshRender);
	pEffect->AddComponent<CAnimator_2D>(new CAnimator_2D);
	//pEffect->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);

	pEffect->Transform()->SetLocalScale(Vec3(3000.f * 1.7f, 3000.f, 1.f));
	pEffect->Transform()->SetLocalRot(Vec3(1.57f, 0.f, 0.f));

	pEffect->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D_One");
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	info = {};
	info.strTexName = L"Flare_2.png";
	info.strAnimName = L"Flare_2";
	info.iContinue = 0;
	info.iFrameCountX = 5;
	info.iFrameCountY = 5;
	info.vLeftTop.x = 0.f;
	info.vLeftTop.y = 0.f;
	info.vSize.x = 888.f;
	info.vSize.y = 500.f;

	strPath = CPathMgr::GetResPath();
	strPath += L"Texture\\Effect";

	pEffect->Animator_2D()->LoadAnimation_2D(&info, strPath);
	pEffect->Animator_2D()->SetTerm(L"Flare_2", 0.02f);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Flare2_Effect", L"Default", pEffect))
		delete pEffect;

	m_strKey[(UINT)EFFECT_TYPE::FLARE_2] = L"Flare2_Effect";
}

CPrefab * CEffectMgr::GetEffect(const EFFECT_TYPE& _eType)
{
	CPrefab* pTest = (CPrefab*)CResMgr::GetInst()->Load<CPrefab>(m_strKey[(UINT)_eType]);

	return pTest;
}

void CEffectMgr::LoadEffect(const wstring & _strEffectFolder)
{
	if (NULL == CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect"))
		return;

	wstring strFolderPath = {};
	strFolderPath = CPathMgr::GetResPath();
	strFolderPath += L"Save\\Effect\\" + _strEffectFolder;

	WIN32_FIND_DATA tData = {};
	HANDLE hHandle = FindFirstFile(wstring(strFolderPath + L"\\*.*").c_str(), &tData);

	if (INVALID_HANDLE_VALUE == hHandle)
	{
		MessageBox(NULL, L"존재하지 않는 폴더입니다", L"이펙트 로드 실패", MB_OK);
		assert(NULL);
	}

	CGameObject* pEffect;
	vector<CGameObject*> vecEffect;
	wstring strExt;
	while (true)
	{
		strExt = CPathMgr::GetExt(tData.cFileName);
		if (L".goc" == strExt)
		{
			vecEffect.clear();
			pEffect = CSaveLoadMgr::GetInst()->LoadGameObject(wstring(strFolderPath +L"\\"+ tData.cFileName));
			pEffect->Transform()->SetLocalPos(Vec3(0, 0, 0));
			pEffect->ClearParent();
			CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect")->AddGameObject(pEffect);
			pEffect->SetTag(CPathMgr::GetFileName(tData.cFileName));
			pEffect->SetActive(false);
			vecEffect.push_back(pEffect);
			for (int i = 0; i < 10; ++i)
			{
				pEffect = pEffect->Clone();
				vecEffect.push_back(pEffect);
				CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect")->AddGameObject(pEffect);
			}
			m_mapEffect.insert(make_pair(pEffect->GetTag(), vecEffect));
		}

		if (0 == FindNextFile(hHandle, &tData))
			break;
	}
	FindClose(hHandle);
}


void CEffectMgr::Save(FILE * _pFile)
{
	UINT iCount = 0;

	for (UINT i = 0; i < (UINT)EFFECT_TYPE::END; ++i)
	{
		if (false == m_strKey[i].empty())
		{
			++iCount;
		}
	}
	fwrite(&iCount, sizeof(UINT), 1, _pFile);
	
	for (UINT i = 0; i < iCount; ++i)
	{	
		SaveWString(m_strKey[i], _pFile);
	}
}

void CEffectMgr::Load(FILE * _pFile)
{
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		m_strKey[i] = LoadWString(_pFile);
	}

	g_pThis = this;
	//
	Init();
}