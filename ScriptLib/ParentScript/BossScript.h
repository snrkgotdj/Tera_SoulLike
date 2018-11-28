#pragma once
#include "Script.h"

class CPlayerScript;

enum class BOSS_STATE
{
	SPAWN,
	WAIT,
	WALK,
	HIT,
	ATK_01,
	ATK_02,
	ATK_03,
	ATK_04,
	ATK_ULTRA,
	DEAD,
	END
};

class CBossScript :
	public CScript
{
protected:
	BOSS_STATE		m_eState;
	CGameObject*	m_pPlayerObj;
	CPlayerScript*	m_pPlayerScript;

	Vec3			m_vPos;
	Vec3			m_vRot;
	Vec3			m_vDir;

	float			m_fSpeed;
	float			m_fDist_Att;
	float			m_fWaitTime;

	int				m_iCurFrameIdx;

	int				m_iHP;
	int				m_iMaxHP;

	CGameObject*	m_pHPBar;
	float			m_fAccDT;

	bool			m_bBurnOut;
	int				m_iDeadIdx;

	bool			m_bHit;
	float			m_fHitTime;

public:
	virtual void Awake();
	virtual void Start();
	virtual int Update();

protected:
	virtual void Spawn();
	virtual void Wait();
	virtual void Walk();
	virtual void Hit();
	virtual void Atk_01() {};
	virtual void Atk_02() {};
	virtual void Atk_03() {};
	virtual void Atk_04() {};
	virtual void Atk_Ultra() {};
	virtual void Dead();

protected:
	void SetTrailActive(bool _bActive);

public:
	BOSS_STATE GetState() { return m_eState; }
	void GetDamage(UINT _iDamage, bool _bBlood = false);

private:
	void UpdateHpBar();
	void BurnOut(float _fBurnTime);

public:
	CLONE(CBossScript)

public:
	CBossScript();
	virtual ~CBossScript();
};

