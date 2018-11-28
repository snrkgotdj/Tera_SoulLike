#pragma once
#include "ParentScript\SkillScript.h"

class CEnergyBall :
	public CSkillScript
{
private:
	bool	m_bStart;
	bool	m_bEnd;

public:
	virtual int Update();

public:
	virtual void OnCollisionEnter(CCollider* _pOther);

protected:
	virtual void CastSkill();
	virtual void SkillStart();

public:
	CLONE(CEnergyBall)

public:
	CEnergyBall();
	virtual ~CEnergyBall();
};