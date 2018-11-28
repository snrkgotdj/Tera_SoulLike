#include "SunScript.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Light.h"

#include "MgrScript/EffectMgr.h"
#include "ShockWave.h"
#include "MgrScript/SkillMgr.h"

CSunScript::CSunScript()
	:m_pBoss(nullptr)
	, m_fAccTime(0.f)
	, m_fLifeTime(0.f)
	, m_fSpeed(0.f)
	, m_bOnce(false)
{
}

CSunScript::~CSunScript()
{
}

int CSunScript::Update()
{
	Vec3 vScale = Transform()->GetLocalScale();
	Vec3 vPos = Transform()->GetLocalPos();
	m_fAccTime += DT();

	if (vScale.x < 1200)
	{
		vScale += 800 * DT();
		Vec3 vPlyaerPos = CSceneMgr::GetInst()->GetPlayerObj()->Transform()->GetLocalPos();
		m_vPlayerPos = vPlyaerPos;
		vPlyaerPos.y += 4;
		vPos.y += 80 * DT();
		
		if (m_fAccTime >= 1.2)
		{
			CEffectMgr::GetInst()->ActiveEffect(L"SunTarget", vPlyaerPos, Vec3(800, 800, 800), 0.05, true);
			m_fAccTime = 0.f;
		}
	}
	else if (vPos.y >= 5)
	{
		if (m_pBoss != nullptr)
		{
			Vec3 vDir = m_pBoss->Transform()->GetDir(DIR_TYPE::RIGHT);
			vDir = m_vPlayerPos - vPos; vDir.Normalize();
			
			m_fSpeed += 5000 * DT();
			vPos += vDir * m_fSpeed * DT();
		}
	}

	else
	{
		if (m_bOnce == false)
		{
			CShockWave* pScript = (CShockWave*)CSkillMgr::GetInst()->UseSkill(SKILL_TYPE::SHOCKWAVE, vPos, GetGameObject());
			pScript->Init(vPos, true);
			m_bOnce = true;
		}
		
		m_fLifeTime += DT();
		if (m_fAccTime >= 0.06)
		{
			Vec3 vBombPos = vPos;
			for (int i = 0; i < 3; ++i)
			{
				vBombPos.x += getRandNum_float(-200, 200);
				vBombPos.z += getRandNum_float(-200, 200);
				CEffectMgr::GetInst()->ActiveEffect(L"Bomb_01", vBombPos, Vec3(1000, 1000, 1000), 0.01, true);
				vBombPos.y += 15;
				CEffectMgr::GetInst()->ActiveEffect(L"Bomb_04", vBombPos, Vec3(300, 300, 300), 0.02, true);

				m_fAccTime = 0.f;
			}
		}

		if (m_fLifeTime >= 0.4)
		{
			GetGameObject()->SetActive(false);
			m_fSpeed = 0.f;
			m_fLifeTime = 0.f;
			m_bOnce = false;
		}
	}

	Transform()->SetLocalScale(vScale);
	Transform()->SetLocalPos(vPos);
	Light()->SetLightPos(Vec4(vPos.x, vPos.y, vPos.z, 1.f));
	Light()->SetRange(vScale.x * 15);
	return 0;
}

