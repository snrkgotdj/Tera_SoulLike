#pragma once
#include "Script.h"
#include "Navigation.h"

class CThirdCamScript;

enum PLAYER_STATE
{
	WALK,
	WAIT,
	RUN,
	HIT,
	ATK_01,
	ATK_01_1,
	ATK_02,
	ATK_03,
	ATK_04_1,
	ATK_04_2,
	ATK_04_3,
	READY_ACTION,
	DEAD,
	END,
};

enum class DIR_STATE
{
	FRONT,
	FRONT_RIGHT,
	RIGHT,
	BACK_RIGHT,
	BACK,
	BACK_LEFT,
	LEFT,
	FRONT_LEFT,
	END
};

class CPlayerScript :
	public CScript
{
protected:
	PLAYER_STATE		m_eState;
	bool				m_bFootOn;

	Vec3				m_vPos;
	Vec3				m_vBeforePos;
	Vec3				m_vRot;
	Vec3				m_vScale;

	int					m_iMaxHP;
	int					m_iHP;
	CNavigation			m_cNavigation;

	bool				m_bStun;
	float				m_fStunTime;
	float				m_fStunCoolTime;

	CThirdCamScript*	m_pCam;
	DIR_STATE			m_DirState;

	bool				m_bSuperArmor;
	CGameObject*		m_pBloodObj;

	int					m_iCurAnimIdx;
	float				m_fAccParticle;
	float				m_fAccDust;

	CGameObject*		m_pLight;
	float				m_fWalkSpeed;

	CGameObject*		m_pHPBar;
	float				m_fAccDT;

	CGameObject*		m_pRock;
	bool				m_bRockCol;

public:
	virtual void Awake();
	virtual void Start();
	virtual int Update();
	virtual int DelayUpdate();
	virtual int LateUpdate();

public:
	virtual void OnCollisionEnter(CCollider* _pOther);
	virtual void OnCollision(CCollider* _pOther);
	virtual void OnCollisionExit(CCollider* _pOther);

protected:
	virtual void Wait();
	virtual void Walk();
	virtual void Run();
	virtual void Hit();
	virtual void Atk_01();
	virtual void Atk_01_1() {};
	virtual void Atk_02();
	virtual void Atk_03();
	virtual void Atk_04_1() {};
	virtual void Atk_04_2() {};
	virtual void Atk_04_3() {};
	virtual void ReadyAction() {};
	virtual void Dead();

public:
	PLAYER_STATE GetPlayerState() { return m_eState; }
	void GetDamage(UINT _iDamage, bool _bBlood = false, float _fStun = 0.f);
	const PLAYER_STATE& GetState() { return m_eState; }
	int GetCurAnimIdx() { return m_iCurAnimIdx; }
	void OnPointLight(const Vec3& _vPos, const Vec3& _vCol);
	bool CheckHide(const Vec3& _vBossPos);
	
protected:
	void SetTrailActive(bool _bActive);
	void CheckAttack();
	void SplatParticle();

private:
	void UpdateHpBar();

public:
	CLONE(CPlayerScript)

public:
	CPlayerScript();
	virtual ~CPlayerScript();
};

