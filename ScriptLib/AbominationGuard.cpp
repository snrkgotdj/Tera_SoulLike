#include "AbominationGuard.h"

#include "Animator_3D.h"
#include "Audio.h"

#include "ParentScript\PlayerScript.h"

CAbominationGuard::CAbominationGuard()
{
	m_ID = SCRIPT_BLOODGOLEM;
}


CAbominationGuard::~CAbominationGuard()
{
}

void CAbominationGuard::Start()
{
	CMonsterScript::Start();

	Animator_3D()->AddAnimation(L"Atk_1", 0, 30, 1.f);
	Animator_3D()->AddAnimation(L"Dead", 135, 262, 1.f);
	Animator_3D()->AddAnimation(L"Hit", 442, 472, 1.f);
	Animator_3D()->AddAnimation(L"Wait", 1491, 1557, 1.f);
	Animator_3D()->AddAnimation(L"Walk", 1558, 1599, 2.f);

	SetHPBarPosY(130.f);

	m_iDeadIdx = 262;
}

void CAbominationGuard::Walk()
{
	CMonsterScript::Walk();

	if (1558 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 1))
		{
		case 0:
			Audio()->Play(L"Guard_Walk01", 1);
			break;
		case 1:
			Audio()->Play(L"Guard_Walk02", 1);
			break;
		}
	}
}

void CAbominationGuard::Atk_01()
{
	CMonsterScript::Atk_01();

	if (0 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 1))
		{
		case 0:
			Audio()->Play(L"Guard_Attack01_1", 1);
			break;
		case 1:
			Audio()->Play(L"Guard_Attack01_2", 1);
			break;
		}
	}

	if (false == m_bAttack && Animator_3D()->GetCurAnimPrecent() >= 0.5f)
	{
		if (m_fAttackDist >= m_fPlayerDist)
		{
			m_pPlayerScript->GetDamage(5);
		}
		m_bAttack = true;
	}
}

void CAbominationGuard::Hit()
{
	CMonsterScript::Hit();

	if (442 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 2))
		{
		case 0:
			Audio()->Play(L"Guard_Damage01", 1);
			break;
		case 1:
			Audio()->Play(L"Guard_Damage02", 1);
			break;
		case 2:
			Audio()->Play(L"Guard_Damage03", 1);
			break;
		}
	}
}

void CAbominationGuard::Dead()
{
	CMonsterScript::Dead();

	if (m_iDeadIdx == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 1))
		{
		case 0:
			Audio()->Play(L"Guard_Death01", 1);
			break;
		case 1:
			Audio()->Play(L"Guard_Death02", 1);
			break;
		}
	}
}
