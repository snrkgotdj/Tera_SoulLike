#pragma once
#include "Script.h"
class CBurnOutScript :
	public CScript
{
private:
	bool m_bBurnOut;
	float m_fAccTime;
	float m_fBurnTime;


public:
	void SetBurnTime(float _fTime) { m_fBurnTime = _fTime; }

public:
	virtual int Update();

public:
	CLONE(CBurnOutScript)

public:
	CBurnOutScript();
	virtual ~CBurnOutScript();
};

