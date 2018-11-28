#include "MonsterScript.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"
#include "Transform.h"
#include "Animator_3D.h"
#include "Audio.h"

#include "../MgrScript/GameSystemMgr.h"
#include "../MgrScript/EffectMgr.h"
#include "PlayerScript.h"
#include "../TileEffect.h"
#include "..\BurnOutScript.h"
#include "../BillBoardScript.h"

CMonsterScript::CMonsterScript()
	: m_eState(MONSTER_STATE::WAIT)
	, m_pPlayerObj(NULL)
	, m_pPlayerScript(NULL)
	, m_iMaxHP(100)
	, m_iHP(100)
	, m_fSpeed(100.f)
	, m_fPlayerDist(0.f)
	, m_fWaitDist(5000000.f)
	, m_fAttackDist(70.f)
	, m_bRigidity(false)
	, m_fRigidityTime(0.f)
	, m_fRigidityCoolTime(0.5f)
	, m_bAttack(false)
	, m_pSkill(NULL)
	, m_pBloodObj(NULL)
	, m_bHit(false)
	, m_fHitTime(0.f)
	, m_bAttWait(false)
	, m_fAttTime(0.f)
	, m_fAttCoolTime(0.5f)
	, m_bNoHit(false)
	, m_bBurnOUt(false)
	, m_pHPBar(NULL)
	, m_fAccDT(0.f)
	, m_iDeadIdx(0)
{
	m_ID = SCRIPT_MONSTER;
}

CMonsterScript::~CMonsterScript()
{
	if (CSceneMgr::GetInst()->IsPlayMode())
	{
		m_pHPBar->Destroy();
	}
}

void CMonsterScript::Awake()
{
	m_pHPBar = new CGameObject();
	m_pHPBar->SetTag(L"HPBar");
	m_pHPBar->AddComponent<CTransform>(new CTransform);
	m_pHPBar->AddComponent<CMeshRender>(new CMeshRender);
	CBillBoardScript* pBill = new CBillBoardScript;
	pBill->SetYType();
	m_pHPBar->AddScript(SCRIPT_BILLBOARD, pBill);

	m_pHPBar->Transform()->SetLocalScale(Vec3(100.f, 10.f, 100.f));

	m_pHPBar->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_HP"));
	CMaterial* pMtrl = m_pHPBar->MeshRender()->GetMaterial();
	m_pHPBar->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));

	m_pHPBar->Awake();

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(m_pHPBar);
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"HP", L"Texture\\HP.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
}

void CMonsterScript::Start()
{
	m_pPlayerObj = CSceneMgr::GetInst()->GetPlayerObj();
	m_pPlayerScript = (CPlayerScript*)m_pPlayerObj->GetComponent<CPlayerScript>();

	m_pHPBar->Start();
	m_pHPBar->SetActive(false);

	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));

	int iSize = MeshRender()->GetMaterialSize();
	for (int i = 0; i < iSize; ++i)
		MeshRender()->GetMaterial(i);
}

int CMonsterScript::Update()
{
	if (false == m_pPlayerObj->IsActive())
	{
		Wait();
		return 0;
	}
	else if (0 == m_iHP)
	{
		Dead();
		return 0;
	}

	m_vPos = Transform()->GetLocalPos();
	m_vRot = Transform()->GetLocalRot();

	Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
	Vec3 vDir = vPlayerPos - m_vPos;
	m_fPlayerDist = vDir.Distance();
	vDir.Normalize();

	if (false == m_bAttack)
	{
		Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);
		Vec3 vCross = vFront.Cross(vDir);
		float fRotY = vFront.Dot(vDir);

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

	if (m_bAttWait)
	{
		m_fAttTime += DT();

		if (m_fAttTime >= m_fAttCoolTime)
		{
			m_fAttTime = 0.f;
			m_bAttWait = false;
		}
	}

	if (MONSTER_STATE::HIT != m_eState && MONSTER_STATE::ATK_01 != m_eState)
	{
		if (m_fAttackDist < m_fPlayerDist && m_fPlayerDist <= m_fWaitDist)
		{
			m_eState = MONSTER_STATE::WALK;
		}
		else
		{
			if (m_bAttWait)
				m_eState = MONSTER_STATE::WAIT;
			else
				m_eState = MONSTER_STATE::ATK_01;
		}
	}
	else if (false == m_bRigidity)
	{
		m_eState = MONSTER_STATE::ATK_01;
	}

	switch (m_eState)
	{
	case MONSTER_STATE::WAIT:		Wait();		break;
	case MONSTER_STATE::WALK:		Walk();		break;
	case MONSTER_STATE::HIT:		Hit();		break;
	case MONSTER_STATE::ATK_01:		Atk_01();	break;
	default:
		break;
	}

	m_vPos.y = 0.f;

	Transform()->SetLocalPos(m_vPos);
	Transform()->SetLocalRot(m_vRot);

	if (NULL != m_pBloodObj && m_pBloodObj->IsDestroy())
		m_pBloodObj = NULL;

	if (m_bHit)
		m_fHitTime += DT();

	if (m_fHitTime >= 0.05f)
		m_bHit = false;

	UpdateHpBar();

	return 0;
}

void CMonsterScript::OnCollisionEnter(CCollider * _pOther)
{
	int iCurAnimIdx = m_pPlayerScript->Animator_3D()->GetCurFrameIdx();

	if (L"Sword" == _pOther->GetGameObject()->GetTag())
	{
		int iCurAnimIdx = m_pPlayerScript->Animator_3D()->GetCurFrameIdx();

		if (PLAYER_STATE::ATK_01 == m_pPlayerScript->GetState() && iCurAnimIdx < 48 && iCurAnimIdx >= 28)
		{
			m_pPlayerObj->Animator_3D()->SetRSSpeed(0.0f, 0.04f);
			GetDamage(20, true);
		}

		else if (PLAYER_STATE::ATK_01_1 == m_pPlayerScript->GetState())
		{
			m_pPlayerObj->Animator_3D()->SetRSSpeed(0.0f, 0.04f);
			GetDamage(20, true);
		}

		if (PLAYER_STATE::ATK_02 == m_pPlayerScript->GetState() && 250 > iCurAnimIdx)
			GetDamage(20, true);

		if (PLAYER_STATE::ATK_04_2 == m_pPlayerScript->GetState())
		{
			GetDamage(1, true);
		}
	}
}

void CMonsterScript::OnCollision(CCollider * _pOther)
{
	if (_pOther->GetGameObject() != m_pPlayerObj && MONSTER_STATE::WALK == m_eState)
	{
		Vec3 vColPos = _pOther->GetGameObject()->Transform()->GetLocalPos();
		Vec3 vDist = vColPos - m_vPos;

		vDist.Normalize();
		m_vPos -= vDist * 100.f * DT();
		Transform()->SetLocalPos(m_vPos);
	}
}

void CMonsterScript::Wait()
{
	Animator_3D()->PlayAnimation(L"Wait");
}

void CMonsterScript::Walk()
{
	Animator_3D()->PlayAnimation(L"Walk");

	Vec3 vDir = m_pPlayerObj->Transform()->GetLocalPos() - m_vPos;
	float fDist = vDir.Distance();
	vDir.Normalize();
	Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);

	m_vPos += vFront * m_fSpeed * DT();

	float fRad = acos(vDir.Dot(vFront));
	Vec3 vCross = vDir.Cross(vFront);
	if (vCross.Distance() != 0)
		vCross.Normalize();

	if (fRad < 0.2 && fRad > -0.2)
		return;

	if (vCross.y < 0)
	{
		if (fRad > 0)
			m_vRot.y += DT();

		else if (fRad < 0)
			m_vRot.y -= DT();
	}

	else if (vCross.y >= 0)
	{
		if (fRad > 0)
			m_vRot.y -= DT();

		else if (fRad < 0)
			m_vRot.y += DT();
	}
}

void CMonsterScript::Hit()
{
	Animator_3D()->PlayAnimation(L"Hit");

	if (Animator_3D()->IsAnimFinish())
		m_eState = MONSTER_STATE::END;

	m_fRigidityTime += DT();
	if (m_fRigidityTime >= m_fRigidityCoolTime)
	{
		m_fRigidityTime = 0.f;
		m_bRigidity = false;
	}
}

void CMonsterScript::Atk_01()
{
	if (m_bAttack && Animator_3D()->IsAnimFinish())
	{
		Animator_3D()->PlayAnimation(L"Wait");
		m_eState = MONSTER_STATE::END;
		m_bAttack = false;
		m_bAttWait = true;
		return;
	}

	Animator_3D()->PlayAnimation(L"Atk_1");
}

void CMonsterScript::Dead()
{
	if (Animator_3D()->GetCurFrameIdx() == m_iDeadIdx - 1 && false == m_bBurnOUt)
	{
		BurnOut(0.3f);
	}
}

void CMonsterScript::BurnOut(float _fBurnTime)
{
	m_bBurnOUt = true;
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

void CMonsterScript::GetDamage(UINT _iDamage, bool _bBlood, float _fRigidity)
{
	if (m_bHit || m_iHP <= 0)
		return;

	if (m_iHP == m_iMaxHP)
		m_pHPBar->SetActive(true);

	m_iHP -= _iDamage;

	if (false == m_bNoHit)
		m_eState = MONSTER_STATE::HIT;

	m_bRigidity = true;

	if (_bBlood)
	{
		Vec3 vPos = m_pPlayerObj->Transform()->GetWorldPos();			vPos.y += 200;
		Vec3 vDir = m_vPos - vPos;			vDir.Normalize();			vPos += vDir * 130;

		CEffectMgr::GetInst()->ActiveEffect(L"Dust_01", vPos, Vec3(600, 600, 600), 0.03f)->Audio()/*->Play(L"Hit_01", 1)*/;
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
		CGameSystemMgr::GetInst()->MonsterDie();

		UINT iSize = MeshRender()->GetMaterialSize();

		for (UINT i = 0; i < iSize; ++i)
		{
			CTexture* pTex = NULL;
			MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::SP_TEX_3, &pTex);
		}

		m_pHPBar->SetActive(false);
	}	

	m_bHit = true;
}

void CMonsterScript::SetHPBarPosY(float _fPosY)
{
	Vec3 vPos = m_pHPBar->Transform()->GetLocalPos();
	vPos.y = _fPosY;
	m_pHPBar->Transform()->SetLocalPos(vPos);
}

void CMonsterScript::UpdateHpBar()
{
	if (false == m_pHPBar->IsActive())
		return;

	m_pHPBar->Update();

	Vec3 vPos = m_pHPBar->Transform()->GetLocalPos();
	vPos.x = Transform()->GetLocalPos().x;
	vPos.z = Transform()->GetLocalPos().z;
	m_pHPBar->Transform()->SetLocalPos(vPos);

	float fData = (float)m_iHP / (float)m_iMaxHP;
	m_pHPBar->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);
	m_fAccDT += DT();
	m_pHPBar->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &m_fAccDT);
}	