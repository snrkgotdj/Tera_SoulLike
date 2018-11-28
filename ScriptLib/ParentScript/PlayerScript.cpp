#include "PlayerScript.h"

#include "SceneMgr.h"

#include "GameObject.h"
#include "Animator_3D.h"
#include "Terrain.h"
#include "Trail.h"
#include "Audio.h"
#include "Light.h"

#include "..\ThirdCamScript.h"
#include "..\MgrScript\EffectMgr.h"
#include "..\MgrScript\GameSystemMgr.h"

#include "BossScript.h"

CPlayerScript::CPlayerScript()
	: m_eState(PLAYER_STATE::WAIT)
	, m_bFootOn(false)
	, m_iMaxHP(100)
	, m_iHP(m_iMaxHP)
	, m_bStun(false)
	, m_fStunTime(0.f)
	, m_fStunCoolTime(0.f)
	, m_pCam(NULL)
	, m_bSuperArmor(false)
	, m_pBloodObj(false)
	, m_fAccParticle(0.f)
	, m_fAccDust(0.f)
	, m_pLight(NULL)
	, m_fWalkSpeed(300)
	, m_pHPBar(NULL)
	, m_fAccDT(0.f)
	, m_pRock(NULL)
	, m_bRockCol(false)
{
}

CPlayerScript::~CPlayerScript()
{
}

void CPlayerScript::Awake()
{
	CGameObject* pUI = new CGameObject();
	pUI->SetTag(L"HPBar");
	pUI->AddComponent<CTransform>(new CTransform);
	pUI->AddComponent<CMeshRender>(new CMeshRender);

	pUI->Transform()->SetLocalPos(Vec3(-530.f, -350.f, 0.f));
	pUI->Transform()->SetLocalScale(Vec3(500.f, 150.f, 1.f));

	pUI->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));
	pUI->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D"));
	pUI->MeshRender()->GetMaterial();

	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"UI_BG", L"Texture\\UI\\UI_BG.png");
	pUI->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
	float fData = 1;
	//pUI->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"UI");
	pLayer->AddGameObject(pUI);

	pCurScene->GetUIObject()->AddChildObj(pUI);

	pUI = new CGameObject();
	pUI->SetTag(L"MP");
	pUI->AddComponent<CTransform>(new CTransform);
	pUI->AddComponent<CMeshRender>(new CMeshRender);

	pUI->Transform()->SetLocalPos(Vec3(-490.f, -357.f, 0.f));
	pUI->Transform()->SetLocalScale(Vec3(201.f, 23.f, 1.f));

	pUI->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));
	pUI->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_HP"));
	pUI->MeshRender()->GetMaterial();
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Mana", L"Texture\\UI\\Mana.png");
	pUI->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
	pUI->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);

	pLayer->AddGameObject(pUI);
	pCurScene->GetUIObject()->AddChildObj(pUI);

	m_pHPBar = new CGameObject;
	m_pHPBar->SetTag(L"HP");
	m_pHPBar->AddComponent<CTransform>(new CTransform);
	m_pHPBar->AddComponent<CMeshRender>(new CMeshRender);

	m_pHPBar->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_HP"));
	CMaterial* pMtrl = m_pHPBar->MeshRender()->GetMaterial();
	m_pHPBar->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));

	pLayer->AddGameObject(m_pHPBar);
	pCurScene->GetUIObject()->AddChildObj(m_pHPBar);

	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Health", L"Texture\\UI\\Health.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);

	m_pHPBar->Transform()->SetLocalPos(Vec3(-470.f, -325.f, 0.01f));
	m_pHPBar->Transform()->SetLocalScale(Vec3(260.f, 30.f, 1.f));
}

void CPlayerScript::Start()
{
	CGameObject* pObj= CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);
	m_pCam = (CThirdCamScript*)pObj->FindScript(SCRIPT_THIRDCAMERA);
	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));
}

int CPlayerScript::Update()
{
	m_vPos = Transform()->GetLocalPos();
	m_vRot = Transform()->GetLocalRot();
	m_iCurAnimIdx = Animator_3D()->GetCurFrameIdx();

	if (m_iHP <= 0)
	{
		Animator_3D()->PlayAnimation(L"Dead", true);
		m_eState = PLAYER_STATE::DEAD;
		m_iHP = 0;
	}

	switch (m_eState)
	{
	case PLAYER_STATE::WAIT:				Wait();			break;
	case PLAYER_STATE::WALK:				Walk();			break;
	case PLAYER_STATE::RUN:					Run();			break;
	case PLAYER_STATE::HIT:					Hit();			break;
	case PLAYER_STATE::ATK_01:				Atk_01();		break;
	case PLAYER_STATE::ATK_01_1:			Atk_01_1();		break;
	case PLAYER_STATE::ATK_02:				Atk_02();		break;
	case PLAYER_STATE::ATK_03:				Atk_03();		break;
	case PLAYER_STATE::ATK_04_1:			Atk_04_1();		break;
	case PLAYER_STATE::ATK_04_2:			Atk_04_2();		break;
	case PLAYER_STATE::ATK_04_3:			Atk_04_3();		break;
	case PLAYER_STATE::READY_ACTION:		ReadyAction();	break;
	case PLAYER_STATE::DEAD:				Dead();			break;

	default:
		break;
	}
	
	Transform()->SetLocalPos(m_vPos);
	Transform()->SetLocalRot(m_vRot);
	
	if (m_bStun)
		m_fStunTime += DT();

	if (m_fStunTime >= m_fStunCoolTime && m_fStunCoolTime > 0.f)
	{
		m_fStunTime = 0.f;
		m_fStunCoolTime = 0.f;
		m_bStun = false;
	}

	m_vBeforePos = m_vPos;

	if (NULL != m_pBloodObj && m_pBloodObj->IsDestroy())
		m_pBloodObj = NULL;

	if (NULL != m_pLight && m_pLight->IsActive())
	{
		float fRange = m_pLight->Light()->GetRange();

		fRange -= DT() * 700.f;
		m_pLight->Light()->SetRange(fRange);

		if (fRange <= 10.f)
		{
			m_pLight->SetActive(false);
		}
	}

	if (NULL != m_pRock && m_pRock->IsDestroy())
	{
		m_bRockCol = false;
		m_pRock = NULL;
	}

	UpdateHpBar();

	return 0;
}

int CPlayerScript::DelayUpdate()
{
	return 0;
}

int CPlayerScript::LateUpdate()
{
	CGameObject* pObj = CSceneMgr::GetInst()->GetTerrainObj();

	if (pObj == nullptr)
		return 0;

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vTerrain = pObj->Transform()->GetLocalPos();

	vPos = pObj->Terrain()->GetTerrainPos(vPos);

	Transform()->SetLocalPos(vPos);

	return 0;
}

void CPlayerScript::OnCollisionEnter(CCollider * _pOther)
{
	if (NULL != _pOther->GetParentObject() && L"Boss" == _pOther->GetParentObject()->GetTag())
	{
		CBossScript* pBoss = ((CBossScript*)_pOther->GetParentObject()->GetComponent<CBossScript>());

		if (BOSS_STATE::ATK_01 == pBoss->GetState() && pBoss->Animator_3D()->GetCurAnimPrecent() <= 0.7f)
		{
			pBoss->GetGameObject()->Animator_3D()->SetRSSpeed(0.0f, 0.02f);
			GetDamage(0, true);
		}
		else if (BOSS_STATE::ATK_02 == pBoss->GetState() && pBoss->Animator_3D()->GetCurAnimPrecent() >= 0.4f && false == m_bStun)
		{
			GetDamage(0, false, 2.5f);
		}
		else if (BOSS_STATE::ATK_03 == pBoss->GetState())
		{
			GetDamage(0, true);			// 5대 정도
		}
	}
}

void CPlayerScript::OnCollision(CCollider * _pOther)
{
	if (L"Rock" == _pOther->GetGameObject()->GetLayerName())
	{
		m_pRock = _pOther->GetGameObject();
		m_bRockCol = true;
	}
}

void CPlayerScript::OnCollisionExit(CCollider * _pOther)
{
	if (L"Rock" == _pOther->GetGameObject()->GetLayerName())
	{
		m_pRock = NULL;
		m_bRockCol = false;
	}
}

void CPlayerScript::Wait()
{
	Animator_3D()->PlayAnimation(L"Wait");

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_HOLD) || CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_HOLD)
		|| CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_HOLD) || CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_HOLD))
	{
		m_eState = PLAYER_STATE::WALK;
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LBTN, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_01;
	}
	else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_RBTN, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_02;
	}
}

void CPlayerScript::Walk()
{
	Vec3 vGetPos = Vec3();
	Vec3 vFinalPos = Vec3();

	Vec3 vDir = Vec3();
	Vec3 vCamDir = Vec3();

	UINT iCount = 0;
	if (nullptr == m_pCam)
	{
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * m_fWalkSpeed * DT();
	}
	else
	{
		if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::FRONT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::FRONT);
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::FRONT);
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}
		else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::BACK, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::FRONT) * -1.f;
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::FRONT) * -1;
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}

		if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::LEFT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::RIGHT)* -1.f;
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::RIGHT) * -1;
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}
		else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::RIGHT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::RIGHT);
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::RIGHT);
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}

		if (iCount > 1)
		{
			vDir /= (float)iCount;
			vFinalPos /= (float)iCount;
		}

		m_vPos += vFinalPos;

		Vec3 vCamPos = m_pCam->GetGameObject()->Transform()->GetLocalPos();
		vCamPos += vFinalPos;
		m_pCam->GetGameObject()->Transform()->SetLocalPos(vCamPos);

		// 회전
		vDir = Transform()->GetDir(DIR_TYPE::RIGHT);
		Vec3 vCross = vDir.Cross(vCamDir);
		float fRotY = vDir.Dot(vCamDir);

		if (vCross.y > 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y += DT()*5;
		}
		else if (vCross.y < 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y -= DT() * 5;	
		}
	}
	

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_NONE) && CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_NONE)
		&& CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_NONE) && CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_NONE))
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
	}

	if (KEY_INPUT(KEY_LSHIFT, STATE_HOLD))
	{
		m_eState = PLAYER_STATE::RUN;
		Animator_3D()->PlayAnimation(L"Walk");
	}

	CheckAttack();
}

void CPlayerScript::Run()
{
	Animator_3D()->PlayAnimation(L"Run");

	Vec3 vGetPos = Vec3();
	Vec3 vFinalPos = Vec3();

	Vec3 vDir = Vec3();
	Vec3 vCamDir = Vec3();
	
	UINT iCount = 0;
	if (nullptr == m_pCam)
	{
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * m_fWalkSpeed * 2 * DT();
	}
	else
	{
		if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::FRONT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::FRONT);
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::FRONT);
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}
		else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::BACK, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::FRONT) * -1.f;
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::FRONT) * -1;
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}

		if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::LEFT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::RIGHT);
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::RIGHT) * -1;
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}
		else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_HOLD))
		{
			m_pCam->GetPos(m_eState, DIR_STATE::RIGHT, vGetPos);
			vFinalPos += vGetPos;

			vDir += Transform()->GetDir(DIR_TYPE::RIGHT) * -1.f;
			vCamDir += m_pCam->GetGameObject()->Transform()->GetDir(DIR_TYPE::RIGHT);
			vCamDir.y = 0.f;
			vCamDir.Normalize();

			iCount++;
		}

		if (iCount > 1)
		{
			vDir /= (float)iCount;
			vFinalPos /= (float)iCount;
		}

		m_vPos += vFinalPos;

		Vec3 vCamPos = m_pCam->GetGameObject()->Transform()->GetLocalPos();
		vCamPos += vFinalPos;
		m_pCam->GetGameObject()->Transform()->SetLocalPos(vCamPos);

		vDir = Transform()->GetDir(DIR_TYPE::RIGHT);
		Vec3 vCross = vDir.Cross(vCamDir);
		float fRotY = vDir.Dot(vCamDir);

		if (vCross.y > 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y += DT() * 5;
		}

		else if (vCross.y < 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y -= DT() * 5;
		}
	}
	
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_NONE) && CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_NONE)
		&& CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_NONE) && CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_NONE))
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
	}

	if (KEY_INPUT(KEY_LSHIFT, STATE_NONE))
	{
		m_eState = PLAYER_STATE::WALK;
		Animator_3D()->PlayAnimation(L"Walk");
	}

	CheckAttack();
	SplatParticle();
}

void CPlayerScript::Hit()
{
	CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_BloodScreen");
	CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);

	if (false == m_bStun)
	{
		if (Animator_3D()->IsAnimFinish())
		{
			CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetPrevFilter();

			m_eState = PLAYER_STATE::WAIT;
			Animator_3D()->PlayAnimation(L"Wait");
		}
		else if (Animator_3D()->GetCurAnimPrecent() >= 0.5f)
		{
			if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, STATE_HOLD) || CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, STATE_HOLD)
				|| CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, STATE_HOLD) || CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, STATE_HOLD))
			{
				CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetPrevFilter();
				m_eState = PLAYER_STATE::WALK;
				Animator_3D()->PlayAnimation(L"Walk");
			}

			if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LBTN, STATE_TAB))
			{
				CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetPrevFilter();
				m_eState = PLAYER_STATE::ATK_01;
				Atk_01();
			}
		}		
	}
}

void CPlayerScript::Atk_01()
{
	Animator_3D()->PlayAnimation(L"Atk_1", true);

	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
	}
}

void CPlayerScript::Atk_02()
{
	if (Animator_3D()->IsAnimFinish())
	{
		Animator_3D()->PlayAnimation(L"Wait");
		m_eState = PLAYER_STATE::WAIT;
	}
}

void CPlayerScript::Atk_03()
{
	if (Animator_3D()->IsAnimFinish())
	{
		Animator_3D()->PlayAnimation(L"Wait");
		m_eState = PLAYER_STATE::WAIT;
	}
}

void CPlayerScript::Dead()
{
	CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_BloodScreen");
	CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);

	if (Animator_3D()->IsAnimFinish())
	{
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetPrevFilter();
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		m_iHP = m_iMaxHP;
	}
}

void CPlayerScript::GetDamage(UINT _iDamage, bool _bBlood, float _fStun)
{
	if (m_iHP <= 0)
		return;

	SetTrailActive(false);
	m_iHP -= _iDamage;

	if (m_iHP <= 0)
	{
		Animator_3D()->PlayAnimation(L"Dead", true);
		m_eState = PLAYER_STATE::DEAD;
		m_iHP = 0;
	}
	else
	{
		if (false == m_bSuperArmor)
		{
			if (_fStun > 0.f)
			{
				m_fStunCoolTime = _fStun;
				m_bStun = true;
				Animator_3D()->PlayAnimation(L"Stun");
				m_eState = PLAYER_STATE::HIT;
			}

			if (PLAYER_STATE::ATK_01 > m_eState && PLAYER_STATE::HIT != m_eState)
			{
				if (false == m_bStun)
					Animator_3D()->PlayAnimation(L"Hit");

				m_eState = PLAYER_STATE::HIT;
			}

			if (NULL == m_pBloodObj && _bBlood)
			{
				CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::BLOOD);
				Vec3 vDir = m_pCam->GetGameObject()->Transform()->GetLocalPos() - m_vPos;
				vDir.Normalize();
				Vec3 vPos = m_vPos;
				vPos.y += 30.f;
				vPos.x += vDir.x * 25.f;
				vPos.z += vDir.z * 25.f;
				Instantiate(pPrefab, vPos);
			}
		}
	}
}

void CPlayerScript::OnPointLight(const Vec3& _vPos, const Vec3& _vCol)
{
	if (NULL == m_pLight)
	{
		m_pLight = new CGameObject;
		m_pLight->SetTag(L"Sword_Light");
		m_pLight->AddComponent<CTransform>(new CTransform);
		m_pLight->AddComponent<CMeshRender>(new CMeshRender);
		m_pLight->AddComponent<CLight>(new CLight);

		CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(m_pLight);

		list<CGameObject*> listChild = GetGameObject()->GetChildList();
		list<CGameObject*>::iterator iter = listChild.begin();
		for (; iter != listChild.end(); ++iter)
		{
			if ((*iter)->Trail())
			{
				(*iter)->AddChildObj(m_pLight);
				break;
			}
		}	
	}
	else
	{
		m_pLight->SetActive(true);
	}

	// LightInfo
	tLightCol tCol = {};
	tCol.vDiff = Vec4(_vCol.x, _vCol.y, _vCol.z, 1.f);
	tCol.vSpec = Vec4(1.f, 1.f, 1.f, 1.f);
	tCol.vAmb = Vec4(1.f, 1.f, 1.f, 1.f);

	Vec3 vLightDir = Vec3(1.f, 1.f, 1.f);
	vLightDir.Normalize();

	tLightInfo tInfo(tCol, Vec4(_vPos.x, _vPos.y + 50.f, _vPos.z, 1.f)
		, vLightDir, (UINT)LIGHT_TYPE::POINT);

	m_pLight->Light()->SetLightInfo(tInfo);
	m_pLight->Light()->SetRange(200.f);
}

bool CPlayerScript::CheckHide(const Vec3& _vBossPos)
{
	CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Rock");
	pLayer->DestroyObjAll();

	if (m_bRockCol && NULL != m_pRock)
	{
		Vec3 vRockDir = m_pRock->Transform()->GetLocalPos() - Transform()->GetLocalPos();
		vRockDir.Normalize();

		Vec3 vBossDir = _vBossPos;
		vBossDir -= Transform()->GetLocalPos();
		vBossDir.Normalize();

		float fAngle = acos(vRockDir.Dot(vBossDir));

		if (fAngle <= 0.785)
			return true;
		else
		{
			m_iHP = 0;
			return false;
		}
	}
	else
	{
		m_iHP = 0;
		return false;
	}
}

void CPlayerScript::SetTrailActive(bool _bActive)
{
	vector<CGameObject*> vecTrailChild;

	list<CGameObject*> listChild = GetGameObject()->GetChildList();
	list<CGameObject*>::iterator iter = listChild.begin();
	for (; iter != listChild.end(); ++iter)
	{
		if ((*iter)->Trail())
		{
			vecTrailChild.push_back((*iter));
		}
	}

	for (UINT i = 0; i < vecTrailChild.size(); ++i)
	{
		vecTrailChild[i]->Trail()->SetActive(_bActive);
	}
}

void CPlayerScript::CheckAttack()
{
	if (KEY_INPUT(KEY_1, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_01;
		Animator_3D()->PlayAnimation(L"Atk_01");
	}

	else if (KEY_INPUT(KEY_2, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_02;
		Animator_3D()->PlayAnimation(L"Atk_02");
		SetTrailActive(true);
	}
	else if (KEY_INPUT(KEY_3, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_03;
		Animator_3D()->PlayAnimation(L"Atk_03");
		SetTrailActive(false);
	}
	else if (KEY_INPUT(KEY_4, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_04_1;
		Animator_3D()->PlayAnimation(L"Atk_04_1");
	}
}

void CPlayerScript::SplatParticle()
{
	m_fAccParticle += DT();
	m_fAccDust += DT();
	if (m_fAccParticle >= 0.1)
	{
		m_fAccParticle = 0;
		Vec3 vPos = m_vPos;
		CEffectMgr::GetInst()->ActiveEffect(L"Particle_Blue_02", m_vPos, Vec3(200, 200, 200), 0.03, true);
	}

	if (m_fAccDust >= 0.2)
	{
		m_fAccDust = 0;
		CEffectMgr::GetInst()->ActiveEffect(L"Dust_03", m_vPos, Vec3(200, 200, 200), 0.03, true);
	}
}

void CPlayerScript::UpdateHpBar()
{
	if (false == m_pHPBar->IsActive())
		return;

	m_pHPBar->Update();

	float fData = (float)m_iHP / (float)m_iMaxHP;
	m_pHPBar->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);
	m_fAccDT += DT();
	m_pHPBar->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &m_fAccDT);
}