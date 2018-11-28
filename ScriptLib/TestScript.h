#pragma once
#include "Script.h"
#include "Navigation.h"

enum MON_TYPE
{
	MON_MOVE,
	MON_ATTACK,

	MON_END
};

class CTestScript :
	public CScript
{
private:
	CGameObject*	m_pPlayerObj;
	CNavigation		m_cNavigation;

	float			m_fSpeed;

	Vec3 m_vPos;
	Vec3 m_vRot;
	Vec3 m_vScale;
	Vec3 m_vDir;

public:
	virtual void Awake();
	virtual int Update();
	virtual int DelayUpdate();

public:
	void Move();
	void CalculateDir();

public:
	CLONE(CTestScript)

public:
	CTestScript();
	virtual ~CTestScript();
};