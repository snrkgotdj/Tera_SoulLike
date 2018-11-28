#pragma once
#include "STScript.h"

enum class EFFECT_TYPE
{
	ELECTRIC_CAST,
	ELECTRIC,
	ELECTRIC_END,
	BLOOD,
	BLOOD_TILE,
	WAVE,
	DUST_ANI,
	DUST2_ANI,
	POTAL,
	POTAL_Electric,
	SMOKE1,
	CRACK_TILE,
	//SPWAN,
	FLARE_0,
	FLARE_1,
	FLARE_2,
	END
};

class CEffectMgr :
	public CSTScript<CEffectMgr>
{
private:
	map<wstring, vector<CGameObject*>> m_mapEffect;
	wstring m_strKey[(UINT)EFFECT_TYPE::END];

private:
	int m_iCallCnt;
	bool m_bSoftParticle;

public:
	void ActiveEffect(const wstring& _strEffect, const Vec3& _vecPos);
	CGameObject* ActiveEffect(const wstring& _strEffect, const Vec3& _vecPos, const Vec3& _vScale, float _fTerm = 0, bool _bSoftParticle = false);

	void LoadEffect(const wstring& _strEffectFolder);
	vector<CGameObject*>& FindEffect(const wstring& _strEffect);

	void SetSoftParticle(bool _bOn) { m_bSoftParticle = _bOn; }
public:
	virtual void Awake();
	virtual int Update();

public:
	void Init();
	CPrefab* GetEffect(const EFFECT_TYPE& _eType);

public:
	CLONE(CEffectMgr);
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CEffectMgr();
	virtual ~CEffectMgr();
};

