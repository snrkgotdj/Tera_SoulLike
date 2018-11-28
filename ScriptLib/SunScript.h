#pragma once
#include "Script.h"
class CSunScript :
	public CScript
{
private:
	CGameObject* m_pBoss;
	Vec3 m_vPlayerPos;
	float m_fAccTime;
	float m_fLifeTime;
	float m_fSpeed;
	bool m_bOnce;

public:
	void SetBoss(CGameObject* _pBoss) { m_pBoss = _pBoss; }
	void SetPlayerPos(Vec3 _vPos) { m_vPlayerPos = _vPos; }

public:
	virtual int Update();

	CLONE(CSunScript)

public:
	CSunScript();
	virtual ~CSunScript();
};

