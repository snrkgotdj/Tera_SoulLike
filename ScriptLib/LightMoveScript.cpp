#include "LightMoveScript.h"

#include "SceneMgr.h"
#include "Light.h"

#include "MgrScript\GameSystemMgr.h"

CLightMoveScript::CLightMoveScript()
	:m_pObj(nullptr)
	, m_fDist(2000.f)
{
	m_ID = SCRIPT_LIGHTMOVE;
}

CLightMoveScript::~CLightMoveScript()
{
}


void CLightMoveScript::Start()
{
	m_pObj = CSceneMgr::GetInst()->GetPlayerObj();
}

int CLightMoveScript::Update()
{
	if (m_pObj == nullptr)
		return 0;

	Vec3 vPos = m_pObj->Transform()->GetLocalPos();

	if (CGameSystemMgr::GetInst()->IsBossSpawnTime())
	{
		vPos = Vec3(12000.00, 3000.f, 5000.00);
	}
	else
	{
		Vec3 vLightDir = Light()->GetLightInfo().vLightDir;
		vLightDir.Normalize();

		vPos -= vLightDir * m_fDist;
	}

	Transform()->SetLocalPos(vPos);

	//if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F1, KEY_STATE::STATE_HOLD))
	//{
	//	m_fDist += 10.f;
	//}
	//if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F2, KEY_STATE::STATE_HOLD))
	//{
	//	m_fDist -= 10.f;
	//}

	return 0;
}
