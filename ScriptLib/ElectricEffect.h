#pragma once
#include "ParentScript\EffectScript.h"

class CElectricEffect :
	public CEffectScript
{
public:
	virtual int Update();

public:
	CLONE(CElectricEffect)

public:
	CElectricEffect();
	virtual ~CElectricEffect();
};

