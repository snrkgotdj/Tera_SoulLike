#include "MoveScript.h"
#include "KeyMgr.h"

CMoveScript::CMoveScript()
{
}

CMoveScript::~CMoveScript()
{
}


void CMoveScript::Awake()
{
}

int CMoveScript::Update()
{
	return 0;
}

int CMoveScript::DelayUpdate()
{
	Vec3 vLocal = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_UP, STATE_HOLD))
	{
		vLocal.z += 300 * DT();
	}
	
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_DOWN, STATE_HOLD))
	{
		vLocal.z -= 300 * DT();
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_RIGHT, STATE_HOLD))
	{
		vLocal.x += 300 * DT();
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LEFT, STATE_HOLD))
	{
		vLocal.x -= 300 * DT();
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_1, STATE_HOLD))
	{
		vLocal.y += 300 * DT();
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_2, STATE_HOLD))
	{
		vLocal.y -= 300 * DT();
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_3, STATE_HOLD))
	{
		vRot.y += 1.57f * DT();
	}

	Transform()->SetLocalPos(vLocal);
	Transform()->SetLocalRot(vRot);

	return 0;
}

void CMoveScript::Save(FILE * _pFile)
{
}

void CMoveScript::Load(FILE * _pFile)
{
}