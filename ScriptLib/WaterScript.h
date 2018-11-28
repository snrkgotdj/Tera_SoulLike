#pragma once
#include "Script.h"

class CWaterScript :
	public CScript
{
private:
	float	m_fDelta;
	Vec4	m_vCamWorld;

public:
	virtual void Start();
	virtual int Update();
	virtual int LateUpdate();
	virtual int DelayUpdate();

public:
	CLONE(CWaterScript);

public:
	CWaterScript();
	~CWaterScript();
};

