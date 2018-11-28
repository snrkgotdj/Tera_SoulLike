#pragma once
#include "Script.h"
class CAuraScript :
	public CScript
{
private:
	CGameObject*	m_pAura;
	bool			m_bOn;
	bool			m_bActive;
	float			m_fTime;

private:
	void KeyInput();

public:
	void init();
	virtual void Awake();
	virtual void Start();
	virtual int Update();

public:
	void SetActive(bool	_bTrue) { m_bActive = _bTrue; }
	void SetAuraTex(CTexture* _pTex);

public:
	CLONE(CAuraScript);

public:
	CAuraScript();
	~CAuraScript();
};

