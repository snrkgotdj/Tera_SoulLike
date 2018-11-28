#pragma once
#include "ParentScript\EffectScript.h"

class CBloodEffect :
	public CEffectScript
{
private:
	bool	m_bSetBlood;

public:
	virtual void Start();
	virtual int Update();

public:
	CLONE(CBloodEffect)

public:
	CBloodEffect();
	virtual ~CBloodEffect();
};

