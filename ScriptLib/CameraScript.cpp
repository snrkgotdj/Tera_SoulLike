#include "CameraScript.h"

#include "Texture.h"

CCameraScript::CCameraScript()
	: m_fScale(1.f)
{
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::Awake()
{
	Camera()->SetFar(100000.f);
}

int CCameraScript::Update()
{	
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vFront = Transform()->GetDir(DIR_TYPE::FRONT);
	Vec3 vRight = Transform()->GetDir(DIR_TYPE::RIGHT);

	float fSpeedBoost = 1.f;	
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LSHIFT, KEY_STATE::STATE_HOLD))
	{
		fSpeedBoost = 10.f;
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_W, KEY_STATE::STATE_HOLD))
	{		
		vPos += (vFront * 500.f * DT() * fSpeedBoost);
		Transform()->SetLocalPos(vPos);
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_S, KEY_STATE::STATE_HOLD))
	{
		vPos -= (vFront * 500.f * DT() * fSpeedBoost);
		Transform()->SetLocalPos(vPos);
	}

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_A, KEY_STATE::STATE_HOLD))
	{
		vPos -= (vRight * 500.f * DT()* fSpeedBoost);
		Transform()->SetLocalPos(vPos);
	}
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_D, KEY_STATE::STATE_HOLD))
	{
		vPos += (vRight * 500.f * DT() * fSpeedBoost);
		Transform()->SetLocalPos(vPos);
	}	

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_RBTN, KEY_STATE::STATE_HOLD))
	{
		Vec3 vRot = Transform()->GetLocalRot();

		vRot.y += vMouseDir.x * DT();
		vRot.x -= vMouseDir.y * DT();

		Transform()->SetLocalRot(vRot);
	}

	return 0;
}