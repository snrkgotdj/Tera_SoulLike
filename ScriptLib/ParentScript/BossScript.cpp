#include "BossScript.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"
#include "Animator_3D.h"
#include "Trail.h"
#include "Audio.h"

#include "../MgrScript/EffectMgr.h"
#include "../MgrScript/GameSystemMgr.h"
#include "PlayerScript.h"
#include "../BurnOutScript.h"


CBossScript::CBossScript()
	: m_eState(BOSS_STATE::END)
	, m_pPlayerObj(NULL)
	, m_pPlayerScript(NULL)
	, m_fSpeed(200.f)
	, m_fDist_Att(150.f)
	, m_fWaitTime(0.f)
	, m_iCurFrameIdx(-1)
	, m_iHP(1000)
	, m_iMaxHP(m_iHP)
	, m_pHPBar(NULL)
	, m_fAccDT(0.f)
	, m_bHit(false)
	, m_fHitTime(0.f)
{
}

CBossScript::~CBossScript()
{
	m_pHPBar->Destroy();
}

void CBossScript::Awake()
{
	m_pHPBar = new CGameObject();
	m_pHPBar->SetTag(L"HPBar");
	m_pHPBar->AddComponent<CTransform>(new CTransform);
	m_pHPBar->AddComponent<CMeshRender>(new CMeshRender);

	m_pHPBar->Transform()->SetLocalPos(Vec3(0.f, 400.f, 0.f));
	m_pHPBar->Transform()->SetLocalScale(Vec3(1200.f, 60.f, 1.f));

	m_pHPBar->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));
	m_pHPBar->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Std2D"));
	m_pHPBar->MeshRender()->GetMaterial();

	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"HP_Progressbar", L"Texture\\HP_Progressbar.png");
	m_pHPBar->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &pTex);

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(L"UI");
	pLayer->AddGameObject(m_pHPBar);

	pCurScene->GetUIObject()->AddChildObj(m_pHPBar);

	CGameObject* pHP = new CGameObject;
	pHP->SetTag(L"HP");
	pHP->AddComponent<CTransform>(new CTransform);
	pHP->AddComponent<CMeshRender>(new CMeshRender);

	pHP->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_HP"));
	CMaterial* pMtrl = pHP->MeshRender()->GetMaterial();
	pHP->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));

	pLayer->AddGameObject(pHP);
	m_pHPBar->AddChildObj(pHP);

	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"HP", L"Texture\\HP.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);

	pHP->Transform()->SetLocalPos(Vec3(0.f, -0.02f, 0.f));
	pHP->Transform()->SetLocalScale(Vec3(0.8f, 0.45f, 1.f));

	m_pHPBar->SetActive(false);
}

void CBossScript::Start()
{
	m_pPlayerObj = CSceneMgr::GetInst()->GetPlayerObj();

	if( m_pPlayerObj != nullptr)
		m_pPlayerScript = (CPlayerScript*)m_pPlayerObj->GetComponent<CPlayerScript>();
	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));
}

int CBossScript::Update()
{
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F1, KEY_STATE::STATE_TAB))
	{
		m_eState = BOSS_STATE::ATK_ULTRA;		
	}
	else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F2, KEY_STATE::STATE_TAB))
	{
		m_eState = BOSS_STATE::ATK_02;
	}
	else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F3, KEY_STATE::STATE_TAB))
	{
		m_eState = BOSS_STATE::ATK_03;
	}
	else if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F4, KEY_STATE::STATE_TAB))
	{
		m_eState = BOSS_STATE::ATK_04;
		//m_eState = BOSS_STATE::SPAWN;dw
	}

	UpdateHpBar();

	m_vPos = Transform()->GetLocalPos();
	m_vRot = Transform()->GetLocalRot();
	m_iCurFrameIdx = Animator_3D()->GetCurFrameIdx();

	if (false == m_pPlayerObj->IsActive())
	{
		Wait();
		return 0;
	}
	
	switch (m_eState)
	{
	case BOSS_STATE::SPAWN:			Spawn();		break;
	case BOSS_STATE::WAIT:			Wait();			break;
	case BOSS_STATE::WALK:			Walk();			break;
	case BOSS_STATE::HIT:			Hit();			break;
	case BOSS_STATE::ATK_01:		Atk_01();		break;
	case BOSS_STATE::ATK_02:		Atk_02();		break;
	case BOSS_STATE::ATK_03:		Atk_03();		break;
	case BOSS_STATE::ATK_04:		Atk_04();		break;
	case BOSS_STATE::ATK_ULTRA:		Atk_Ultra();	break;
	case BOSS_STATE::DEAD:			Dead();		    break;
	default:
		break;
	}

	if (m_iHP <= 0)
	{
		return 0;
	}


	if (m_eState != BOSS_STATE::SPAWN)
		m_vPos.y = 0.f;

	Transform()->SetLocalPos(m_vPos);
	Transform()->SetLocalRot(m_vRot);

	if (m_bHit)
	{
		m_fHitTime += DT();
		if (m_fHitTime >= 0.5f)
		{
			m_bHit = false;
			Transform()->SetRimNum(0);
		}
	}

	return 0;
}

void CBossScript::Spawn()
{
	Animator_3D()->PlayAnimation(L"Spawn", true);

	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = BOSS_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
	}
}

void CBossScript::Wait()
{
	Animator_3D()->PlayAnimation(L"Wait");
}

void CBossScript::Walk()
{
	Animator_3D()->PlayAnimation(L"Walk");	

	Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
	Vec3 vDist = vPlayerPos - m_vPos;

	float fDist = vDist.Distance();
	vDist.Normalize();
	Vec3 vDir = vDist;

	Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);
	Vec3 vCross = vFront.Cross(vDir);
	float fRotY = vFront.Dot(vDir);

	if (vCross.y > 0)
	{
		if (fRotY <= 0.99)
			m_vRot.y += DT() * 2;
	}

	else if (vCross.y < 0)
	{
		if (fRotY <= 0.99)
			m_vRot.y -= DT() * 2;
	}

	if (fDist <= m_fDist_Att)
	{
		m_eState = BOSS_STATE::ATK_01;
	}
	else
	{
		m_vPos += vDir * m_fSpeed * DT();
	}
}

void CBossScript::Hit()
{
}

void CBossScript::Dead()
{
	if (m_iCurFrameIdx == m_iDeadIdx - 1 && false == m_bBurnOut)
	{
		BurnOut(0.3f);
	}
}

void CBossScript::SetTrailActive(bool _bActive)
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

void CBossScript::GetDamage(UINT _iDamage, bool _bBlood)
{
	if (m_iHP < 0)
		return;

	//if (m_iHP == m_iMaxHP)
	//	m_pHPBar->SetActive(true);

	m_iHP -= _iDamage;

	if (_bBlood)
	{
		Vec3 vPos = m_pPlayerObj->Transform()->GetWorldPos();			vPos.y += 200;
		Vec3 vDir = m_vPos - vPos;			vDir.Normalize();			vPos += vDir * 130;

		CEffectMgr::GetInst()->ActiveEffect(L"Dust_01", vPos, Vec3(600, 600, 600), 0.03f)->Audio()/*->Play(L"Hit_01", 1)*/;
		//CEffectMgr::GetInst()->ActiveEffect(L"Particle_Blue_01", vPos, Vec3(400, 400, 400), 0.03);
		//CEffectMgr::GetInst()->ActiveEffect(L"Blood", vPos, Vec3(40, 40, 40), 0.02);
		CEffectMgr::GetInst()->ActiveEffect(L"Particle_Red_01", vPos, Vec3(200, 200, 200), 0.02f)->Audio()->Play(L"Hit_02", 1);
		CEffectMgr::GetInst()->ActiveEffect(L"Particle_Electric_01", vPos, Vec3(200, 200, 200), 0.01f)->Audio()/*->Play(L"Hit_03", 1)*/;
		CEffectMgr::GetInst()->ActiveEffect(L"Particle_Electric_02", vPos, Vec3(200, 200, 200), 0.01f)->Audio()->Play(L"Hit_04", 1);
		switch (getRandNum_int(1, 3))
		{
		case 1: CEffectMgr::GetInst()->ActiveEffect(L"Hit_01", vPos, Vec3(600, 600, 600), 0.03f, true);	break;
		case 2: CEffectMgr::GetInst()->ActiveEffect(L"Hit_02", vPos, Vec3(800, 800, 800), 0.03f, true);	break;
		case 3: CEffectMgr::GetInst()->ActiveEffect(L"Hit_03", vPos, Vec3(900, 900, 900), 0.03f, true);	break;
		}

		m_pPlayerScript->OnPointLight(vPos, Vec3(1.f, 1.f, 1.f));
	}

	if (m_iHP <= 0)
	{
		Animator_3D()->PlayAnimation(L"Dead", true);
		m_iHP = 0;
		m_eState = BOSS_STATE::DEAD;
		CGameSystemMgr::GetInst()->MonsterDie();

		UINT iSize = MeshRender()->GetMaterialSize();

		for (UINT i = 0; i < iSize; ++i)
		{
			CTexture* pTex = NULL;
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::SP_TEX_3, &pTex);
		}
	}

	m_bHit = true;
	m_fHitTime = 0.f;
	Transform()->SetRimNum(1);
}

void CBossScript::UpdateHpBar()
{
	if (false == m_pHPBar->IsActive())
		return;

	CGameObject* pHP = m_pHPBar->GetChildList().front();

	pHP->Update();

	float fData = (float)m_iHP / (float)m_iMaxHP;
	pHP->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);
	m_fAccDT += DT();
	pHP->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &m_fAccDT);
}

void CBossScript::BurnOut(float _fBurnTime)
{
	m_bBurnOut = true;
	Animator_3D()->SetStop(true);

	CTexture** ppTex = nullptr;
	CMaterial* pMtrl = nullptr;
	for (UINT i = 0; i < MeshRender()->GetMaterialSize(); ++i)
	{
		ppTex = (CTexture**)MeshRender()->GetSharedMaterial(i)->GetData(SHADER_PARAM::SP_TEX_0);
		pMtrl = CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_BurnOut");
		pMtrl->SetData(SHADER_PARAM::SP_TEX_0, ppTex);
		MeshRender()->SetMaterial(pMtrl, i);
	}
	CBurnOutScript* pBurnScript = new CBurnOutScript;
	pBurnScript->SetBurnTime(_fBurnTime);
	GetGameObject()->AddComponent<CScript>(pBurnScript);
}
