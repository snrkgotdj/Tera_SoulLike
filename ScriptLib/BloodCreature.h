#pragma once
#include "ParentScript\MonsterScript.h"

class CBloodCreature :
	public CMonsterScript
{
public:
	virtual void Start();

private:
	virtual void Walk();
	virtual void Atk_01();
	virtual void Hit();
	virtual void Dead();

public:
	CLONE(CBloodCreature)

public:
	CBloodCreature();
	virtual ~CBloodCreature();
};

