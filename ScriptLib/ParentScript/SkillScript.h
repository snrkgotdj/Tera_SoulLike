#pragma once
#include "Script.h"

class CEffectScript;

class CSkillScript :
	public CScript
{
protected:
	UINT					m_iDamage;
	vector<CEffectScript*>	m_vecEffect;
	Vec3					m_vDir;

	CGameObject*			m_pObj;
	CGameObject*			m_pCastObj;

	float					m_fLifeTime;

public:
	virtual int Update();

protected:
	CEffectScript* EffectCreate(CPrefab* _pPrefab, const SCRIPT_ID& _eID);
	CGameObject* EffectCreate(CPrefab* _pPrefab, const Vec3& _vPos);
	void EffectUpdate();
	void EffectDelete();

public:
	void SetDir(const Vec3& _vDir) { m_vDir = _vDir; m_vDir.Normalize(); }
	void SetObject(CGameObject* _pObj) { m_pObj = _pObj; }
	virtual void CastSkill() {}
	virtual void SkillStart() {}

public:
	virtual CSkillScript* Clone() = 0;

public:
	CSkillScript();
	virtual ~CSkillScript();
};