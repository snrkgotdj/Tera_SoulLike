#include "SkillMgr.h"

#include "ResMgr.h"
#include "MeshData.h"

#include "Collider_3D.h"

#include "../ParentScript/SkillScript.h"
#include "../EnergyBall.h"
#include "../ShockWave.h"

CSkillMgr::CSkillMgr()
{
	m_ID = SCRIPT_SKILLMGR;
}


CSkillMgr::~CSkillMgr()
{
}

void CSkillMgr::Init()
{
	CGameObject* pSkill = new CGameObject;
	pSkill->SetTag(L"Skill_EnergyBall");
	pSkill->AddComponent<CTransform>(new CTransform);
	pSkill->AddComponent<CCollider>(new CCollider_3D);
	pSkill->AddScript(SCRIPT_ID::SCRIPT_ENERGYBALL_SKILL, new CEnergyBall);

	pSkill->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

	pSkill->Collider()->SetColType(COLLIDER_TYPE::SPHERE);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Skill_EnergyBall", L"Default", pSkill))
		delete pSkill;

	m_strKey[(UINT)SKILL_TYPE::ENERGYBALL] = L"Skill_EnergyBall";

	pSkill = new CGameObject;
	pSkill->SetTag(L"Skill_ShockWave");
	pSkill->AddComponent<CTransform>(new CTransform);
	pSkill->AddComponent<CCollider>(new CCollider_3D);
	pSkill->AddScript(SCRIPT_ID::SCRIPT_SHOCKWAVE_SKILL, new CShockWave);

	pSkill->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));

	pSkill->Collider()->SetColType(COLLIDER_TYPE::SPHERE);

	if (RET_FAILED == CResMgr::GetInst()->AddPrefab(L"Skill_ShockWave", L"Default", pSkill))
		delete pSkill;

	m_strKey[(UINT)SKILL_TYPE::SHOCKWAVE] = L"Skill_ShockWave";

}

CSkillScript* CSkillMgr::UseSkill(const SKILL_TYPE& _eType, const Vec3& _vPos, CGameObject* _pObj)
{
	CPrefab* pPrefab = (CPrefab*)CResMgr::GetInst()->Load<CPrefab>(m_strKey[(UINT)_eType]);
	CGameObject* pObj = Instantiate(pPrefab, _vPos);
	CSkillScript* pSkill = ((CSkillScript*)pObj->GetComponent<CSkillScript>());
	pSkill->SetObject(_pObj);

	return pSkill;
}

void CSkillMgr::Save(FILE * _pFile)
{
	UINT iCount = 0;

	for (UINT i = 0; i < (UINT)SKILL_TYPE::END; ++i)
	{
		if (false == m_strKey[i].empty())
		{
			++iCount;
		}
	}
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_strKey[i], _pFile);
	}
}

void CSkillMgr::Load(FILE * _pFile)
{
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		m_strKey[i] = LoadWString(_pFile);
	}

	g_pThis = this;
}