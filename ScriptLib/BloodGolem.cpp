#include "BloodGolem.h"

#include "Animator_3D.h"
#include "Audio.h"

#include "ParentScript\PlayerScript.h"
#include "MgrScript\EffectMgr.h"

CBloodGolem::CBloodGolem()
	: pEffectObj(NULL)
{
	m_ID = SCRIPT_BLOODGOLEM;
	m_fAttackDist = 200.f;
	m_fAttCoolTime = 0.0f;
	m_bNoHit = true;
}

CBloodGolem::~CBloodGolem()
{
}

void CBloodGolem::Start()
{
	CMonsterScript::Start();

	Animator_3D()->AddAnimation(L"Atk_1", 46, 146, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Dead", 590, 842, 2.f);
	Animator_3D()->AddAnimation(L"Wait", 3612, 3660, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Walk", 3661, 3721, 1.f, 0.2f);

	SetHPBarPosY(230.f);

	m_iDeadIdx = 842;

	Transform()->SetRimNum(4);
}

int CBloodGolem::Update()
{
	CMonsterScript::Update();

	if (NULL != pEffectObj && pEffectObj->Animator_2D()->IsAnimFinish())
	{
		pEffectObj->Destroy();
		pEffectObj = NULL;
	}

	return 0;
}

void CBloodGolem::Walk()
{
	CMonsterScript::Walk();

	if (3661 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 1))
		{
		case 0:
			Audio()->Play(L"Golem_run_1", 1);
			break;
		case 1:
			Audio()->Play(L"Golem_run_2", 1);
			break;
		}
	}
}

void CBloodGolem::Atk_01()
{
	CMonsterScript::Atk_01();

	if (46 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 3))
		{
		case 0:
			Audio()->Play(L"Golem_atk01", 1);
			break;
		case 1:
			Audio()->Play(L"Golem_atk02", 1);
			break;
		case 2:
			Audio()->Play(L"Golem_atk03", 1);
			break;
		case 3:
			Audio()->Play(L"Golem_atk04", 1);
			break;
		}
	}

	if (false == m_bAttack && Animator_3D()->GetCurAnimPrecent() >= 0.3f)
	{
		if (m_fAttackDist >= m_fPlayerDist)
		{
			m_pPlayerScript->GetDamage(10, false, 0.2f);						
		}
		m_bAttack = true;
	}
	else if (Animator_3D()->GetCurAnimPrecent() >= 0.35f && Animator_3D()->GetCurAnimPrecent() < 0.4f &&
		NULL == pEffectObj)
	{
		Vec3 vPos = Transform()->GetLocalPos() + (Transform()->GetDir(DIR_TYPE::RIGHT) * 230.f);
		vPos.y += 100.f;

		CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::SMOKE1);

		pEffectObj = Instantiate(pPrefab, vPos, L"Effect");
		pEffectObj->Animator_2D()->PlayAnimation(L"Smoke1", false);
	}
}

void CBloodGolem::Dead()
{
	CMonsterScript::Dead();

	if (m_iDeadIdx == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		Audio()->Play(L"Golem_Death", 1);
	}
}
