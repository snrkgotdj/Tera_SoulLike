#include "WarriorScript.h"

#include "Animator_3D.h"
CWarriorScript::CWarriorScript()
{
	m_ID = SCRIPT_WARRIOR;
}

CWarriorScript::~CWarriorScript()
{
}

void CWarriorScript::Start()
{
	CPlayerScript::Start();

	Animator_3D()->AddAnimation(L"Atk_1", 0, 17, 1.f, 0.1f);
	Animator_3D()->AddAnimation(L"Atk_3", 18, 47);
	Animator_3D()->AddAnimation(L"Atk_2", 48, 149);
	Animator_3D()->AddAnimation(L"Hit", 427, 468, 1.f);
	Animator_3D()->AddAnimation(L"Run", 800, 832, 1.f, 0.1f);
	Animator_3D()->AddAnimation(L"Wait", 1190, 1269, 1.f, 0.1f);
	Animator_3D()->AddAnimation(L"Walk", 1271, 1349, 1.f, 0.1f);
}