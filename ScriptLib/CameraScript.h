#pragma once
#include "Script.h"

class CCameraScript :
	public CScript
{
private:
	float		m_fScale;

public:
	virtual void Awake();
	virtual int Update();

public:
	CLONE(CCameraScript)

public:
	CCameraScript();
	virtual ~CCameraScript();
};

