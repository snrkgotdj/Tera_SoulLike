#pragma once
#include "STScript.h"

enum class STAGE
{
	START,
	STAGE_1,
	STAGE_BOSS,
	END
};

enum class MONSTER_TYPE
{
	GOLEN,
	GUARD,
	CREATURE,
	END,
};

class CAsuraScript;

class CGameSystemMgr :
	public CSTScript<CGameSystemMgr>
{
private:
	STAGE			m_eStage;
	wstring			m_arrMonsterTag[(UINT)MONSTER_TYPE::END];
	UINT			m_iCurMonsterCount;
	CAsuraScript*	m_pAsura;

public:
	virtual void Awake();
	virtual int Update();

public:
	void Init();
	void MonsterDie() { m_iCurMonsterCount--; }
	bool IsBossSpawnTime();

private:
	void StartStage(const STAGE& _eStage);
	void StartBossStage();
	void CreateMonster(const MONSTER_TYPE& _eType, const Vec3& _vPos);

public:
	CLONE(CGameSystemMgr);
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CGameSystemMgr();
	virtual ~CGameSystemMgr();
};