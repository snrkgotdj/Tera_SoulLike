#pragma once
#include "Script.h"

class CToolUIScript :
	public CScript
{
private:
	Matrix		m_matProj;

public:
	virtual void Start();
	virtual int Update();

public:
	void SetUINumber(UINT _i);

public:
	CLONE(CToolUIScript)

public:
	CToolUIScript();
	virtual ~CToolUIScript();
};

