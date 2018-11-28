#pragma once
#include "STScript.h"

class CSkillScript;

enum class SKILL_TYPE
{
	ENERGYBALL,
	SHOCKWAVE,
	END
};

class CSkillMgr :
	public CSTScript<CSkillMgr>
{
private:
	wstring m_strKey[(UINT)SKILL_TYPE::END];

public:
	virtual int Update() { return 0; }

public:
	void Init();
	CSkillScript* UseSkill(const SKILL_TYPE& _eType, const Vec3& _vPos, CGameObject* _pObj);

public:
	CLONE(CSkillMgr);
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CSkillMgr();
	virtual ~CSkillMgr();
};

