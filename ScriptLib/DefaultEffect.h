#pragma once
#include "Script.h"
class CDefaultEffect :
	public CScript
{
private:
	float m_fAccTime;

public:
	virtual void Awake();
	virtual int Update();

public:
	CLONE(CDefaultEffect);

public:
	CDefaultEffect();
	~CDefaultEffect();
};

