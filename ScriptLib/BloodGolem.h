#pragma once
#include "ParentScript\MonsterScript.h"

class CBloodGolem :
	public CMonsterScript
{
private:
	CGameObject* pEffectObj;

public:
	virtual void Start();
	virtual int Update();

private:
	virtual void Walk();
	virtual void Atk_01();
	virtual void Dead();

public:
	CLONE(CBloodGolem)

public:
	CBloodGolem();
	virtual ~CBloodGolem();
};

