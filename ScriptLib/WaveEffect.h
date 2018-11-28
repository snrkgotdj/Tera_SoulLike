#pragma once
#include "ParentScript\EffectScript.h"

class CWaveEffect :
	public CEffectScript
{
private:
	float m_AccDT;

public:
	virtual int Update();

public:
	CLONE(CWaveEffect);

public:
	CWaveEffect();
	virtual ~CWaveEffect();
};

