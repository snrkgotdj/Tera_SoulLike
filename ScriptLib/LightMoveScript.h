#pragma once
#include "Script.h"

class CGameObject;

class CLightMoveScript :
	public CScript
{
private:
	CGameObject*	m_pObj;
	float			m_fDist;

public:
	void SetFollowObj(CGameObject* _pObj) { m_pObj = _pObj; }

public:
	virtual void Start();
	virtual int Update();

public:
	CLONE(CLightMoveScript);

public:
	CLightMoveScript();
	virtual ~CLightMoveScript();
};

