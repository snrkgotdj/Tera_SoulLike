#pragma once
#include "Script.h"

class CPlayerScript;
class CSkillScript;

enum class MONSTER_STATE
{
	WAIT,
	WALK,
	HIT,
	ATK_01,
	DEAD,
	END
};

class CMonsterScript :
	public CScript
{
protected:
	MONSTER_STATE	m_eState;
	CGameObject*	m_pPlayerObj;
	CPlayerScript*	m_pPlayerScript;
	Vec3			m_vPos;
	Vec3			m_vRot;
	Vec3			m_vDir;

	int				m_iMaxHP;
	int				m_iHP;

	float			m_fSpeed;
	float			m_fPlayerDist;
	float			m_fWaitDist;
	float			m_fAttackDist;

	bool			m_bRigidity;
	float			m_fRigidityTime;
	float			m_fRigidityCoolTime;

	bool			m_bAttack;

	CSkillScript*	m_pSkill;

	CGameObject*	m_pBloodObj;
	bool			m_bHit;
	float			m_fHitTime;

	bool			m_bAttWait;
	float			m_fAttTime;
	float			m_fAttCoolTime;

	bool			m_bNoHit;
	bool			m_bBurnOUt;

	CGameObject*	m_pHPBar;
	float			m_fAccDT;

	int				m_iDeadIdx;

public:
	virtual void Awake();
	virtual void Start();
	virtual int Update();

public:
	virtual void OnCollisionEnter(CCollider* _pOther);
	virtual void OnCollision(CCollider* _pOther);

protected:
	virtual void Wait();
	virtual void Walk();
	virtual void Hit();
	virtual void Atk_01();
	virtual void Dead();

	void BurnOut(float _fBurnTime);

public:
	void GetDamage(UINT _iDamage, bool _bBlood = false, float _fRigidity = 0.f);
	void SetHPBarPosY(float _fPosY);

private:
	void UpdateHpBar();

public:
	CLONE(CMonsterScript);

public:
	CMonsterScript();
	virtual ~CMonsterScript();
};