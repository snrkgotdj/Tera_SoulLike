#pragma once
#include "ParentScript\BossScript.h"

class CThirdCamScript;
class CAuraScript;

class CAsuraScript :
	public CBossScript
{
private:
	int						m_iAttack1_State;
	int						m_iAttack2_State;
	int						m_iAttack3_State;
	int						m_iSpawn_State;
	float					m_fWalkTime;

	vector<CGameObject*>	m_vecEffectObj;

	float					m_fFilterRange;
	int						m_iRandInt;

	CThirdCamScript*		m_pCamScript;
	float					m_fFadeTime;
	float					m_fAccTime;
	Vec3					m_vEarthPos;
	int						m_iEarthCount;

	int						m_iVibe;

	CAuraScript*			m_pAura;
	CGameObject*			m_pSwordObj[2];
	CGameObject*			m_pEyeObj[2];

	Vec3					m_vStartPos;

	float					m_fWalkAngle;
	bool					m_bOnce;

	bool					m_bOnLight;
	float					m_fBThreshold;
	float					m_fBScale;

	float					m_fUltraWait;
	float					m_fSunWeight;
	float					m_fAccParticle;

public:
	virtual void Start();
	virtual int Update();
	virtual int DelayUpdate();

public:
	virtual void OnCollisionEnter(CCollider* _pOther);
	virtual void OnCollision(CCollider* _pOther);

private:
	void ClearEffect();
	void SetAuraTex();
	void ShockEarth();
	void CreateSun();
	void SplatParticle();

protected:
	virtual void Spawn();
	virtual void Wait();
	virtual void Walk();
	virtual void Atk_01();
	virtual void Atk_02();
	virtual void Atk_03();
	virtual void Atk_04();
	virtual void Atk_Ultra();
	virtual void Dead();

public:
	CLONE(CAsuraScript);

public:
	CAsuraScript();
	virtual ~CAsuraScript();
};

