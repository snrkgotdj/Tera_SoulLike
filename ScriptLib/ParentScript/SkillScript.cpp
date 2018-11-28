#include "SkillScript.h"

#include "EffectScript.h"

CSkillScript::CSkillScript()
	: m_iDamage(0)
	, m_pObj(NULL)
	, m_pCastObj(NULL)
	, m_fLifeTime(0.f)
{
}

CSkillScript::~CSkillScript()
{
	EffectDelete();
}

int CSkillScript::Update()
{
	m_fLifeTime += DT();
	if (m_fLifeTime >= 30.f)
		GetGameObject()->Destroy();

	return 0;
}

CEffectScript* CSkillScript::EffectCreate(CPrefab* _pPrefab, const SCRIPT_ID & _eID)
{
	CGameObject* pObj = Instantiate(_pPrefab, Transform()->GetLocalPos());
	pObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	CEffectScript* pScript = ((CEffectScript*)pObj->FindScript(_eID));
	pScript->EffectStart();

	return pScript;
}

CGameObject * CSkillScript::EffectCreate(CPrefab * _pPrefab, const Vec3& _vPos)
{
	CGameObject* pObj = Instantiate(_pPrefab, _vPos);
	pObj->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_NONE);

	return pObj;
}

void CSkillScript::EffectUpdate()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vScale = Transform()->GetLocalScale();

	for (UINT i = 0; i < m_vecEffect.size(); ++i)
	{
		m_vecEffect[i]->GetGameObject()->Transform()->SetLocalPos(vPos);
		m_vecEffect[i]->GetGameObject()->Transform()->SetLocalScale(vScale);
	}
}

void CSkillScript::EffectDelete()
{
	for (UINT i = 0; i < m_vecEffect.size(); ++i)
	{
		m_vecEffect[i]->GetGameObject()->Destroy();
	}

	m_vecEffect.clear();
}