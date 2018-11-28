#pragma once
#include "ParentScript\MonsterScript.h"

class CAbominationGuard :
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
	CLONE(CAbominationGuard)

public:
	CAbominationGuard();
	virtual ~CAbominationGuard();
};

