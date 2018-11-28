#pragma once
#include "ParentScript\SkillScript.h"

class CThirdCamScript;

class CShockWave :
	public CSkillScript
{
private:
	CGameObject*		m_pEffectObj1;
	CGameObject*		m_pEffectObj2;
	CThirdCamScript*	m_pCamScript;
	float				m_fTime;

public:
	virtual int Update();

public:
	virtual void OnCollisionEnter(CCollider* _pOther);

public:
	void Init(Vec3 _vEffectPos, bool _bIsRock = false);

public:
	CLONE(CShockWave)

public:
	CShockWave();
	virtual ~CShockWave();
};

