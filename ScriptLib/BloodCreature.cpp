#include "BloodCreature.h"

#include "Animator_3D.h"
#include "Audio.h"

#include "ParentScript\PlayerScript.h"
#include "MgrScript\SoundMgr.h"

CBloodCreature::CBloodCreature()
{
	m_ID = SCRIPT_CREATURE;
	m_fSpeed = 180.f;
}

CBloodCreature::~CBloodCreature()
{
}

void CBloodCreature::Start()
{
	CMonsterScript::Start();

	Animator_3D()->AddAnimation(L"Atk_1", 107, 152, 1.0f);
	Animator_3D()->AddAnimation(L"Dead", 427, 499, 1.f);
	Animator_3D()->AddAnimation(L"Hit", 683, 703, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Wait", 1191, 1230, 1.f);
	Animator_3D()->AddAnimation(L"Walk", 1231, 1271, 2.f);

	SetHPBarPosY(130.f);

	m_iDeadIdx = 499;
}

void CBloodCreature::Walk()
{
	CMonsterScript::Walk();

	if (1231 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 3))
		{
		case 0:
			Audio()->Play(L"Creature_Walk01", 1);
			break;
		case 1:
			Audio()->Play(L"Creature_Walk02", 1);
			break;
		case 2:
			Audio()->Play(L"Creature_Walk03", 1);
			break;
		case 3:
			Audio()->Play(L"Creature_Walk04", 1);
			break;
		}
	}
}

void CBloodCreature::Atk_01()
{
	CMonsterScript::Atk_01();

	if (108 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 1))
		{
		case 0:
			Audio()->Play(L"Creature_Atk01_0", 1);
			break;
		case 1:
			Audio()->Play(L"Creature_Atk01_2", 1);
			break;
		}
	}

	if (false == m_bAttack && Animator_3D()->GetCurAnimPrecent() >= 0.3f)
	{
		if (m_fAttackDist + 80.f >= m_fPlayerDist)
		{
			m_pPlayerScript->GetDamage(5);
		}
		m_bAttack = true;
	}
}

void CBloodCreature::Hit()
{
	CMonsterScript::Hit();

	if (683 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 2))
		{
		case 0:
			Audio()->Play(L"Creature_Damages01", 1);
			break;
		case 1:
			Audio()->Play(L"Creature_Damages02", 1);
			break;
		case 2:
			Audio()->Play(L"Creature_Damages03", 1);
			break;
		}
	}
}

void CBloodCreature::Dead()
{
	CMonsterScript::Dead();

	if (m_iDeadIdx == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		Audio()->Play(L"Creature_Death", 1);
	}
}