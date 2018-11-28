#pragma once
#include "Script.h"

class CEffectScript :
	public CScript
{
protected:
	bool	m_bStart;
	float	m_fLifeTime;

public:
	virtual int Update();

public:
	void EffectStart() { m_bStart = true; }

public:
	CLONE(CEffectScript);

public:
	CEffectScript();
	virtual ~CEffectScript();
};

