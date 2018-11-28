#pragma once
#include "ParentScript\PlayerScript.h"

enum LEADER_TYPE
{
	LEADER_WALK,
	LEADER_WAIT,
	LEADER_ATTACK_01,
	LEADER_ATTACK_02,
	LEADER_ATTACK_03,
	LEADER_ATTACK_04_1,
	LEADER_ATTACK_04_2,
	LEADER_ATTACK_04_3,
	LEADER_END,

};

class CArgoniteLeader :
	public CPlayerScript
{
private:
	bool m_bOnce;
	bool m_bAuraOn;
	float m_fTest;

	CGameObject* m_pSwordObj;

public:
	virtual void Start();

private:
	virtual void Wait();
	virtual void Walk();
	virtual void Run();
	virtual void Atk_01();
	virtual void Atk_01_1();
	virtual void Atk_02();
	virtual void Atk_03();
	virtual void Atk_04_1();
	virtual void Atk_04_2();
	void Atk_04_2Collision();
	virtual void Atk_04_3();
	virtual void ReadyAction();
	virtual void Hit();

private:
	void CreateAfterImage();
	void SetTrailTex();
	void MoveLonger(int _iIdx);
	void SetAura(bool _bTrue);
	void SetAuraTex();

public:
	CLONE(CArgoniteLeader)

public:
	CArgoniteLeader();
	virtual ~CArgoniteLeader();
};

