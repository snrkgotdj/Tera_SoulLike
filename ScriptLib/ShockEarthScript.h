#pragma once
#include "Script.h"
class CShockEarthScript :
	public CScript
{
	float m_fLifeTime;
	bool m_bUp;
	bool m_bRemain;

public:
	void RemainStone(bool _bRemain) { m_bRemain = _bRemain; }

public:
	virtual int Update();

public:
	CLONE(CShockEarthScript)

public:
	CShockEarthScript();
	virtual ~CShockEarthScript();
};

