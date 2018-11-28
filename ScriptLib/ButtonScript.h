#pragma once
#include "Script.h"

enum class BUTTON_STATE
{
	NONE,
	HIGHLIGHTED,
	CLICK
};

typedef std::function<void()> Event;

class CButtonScript :
	public CScript
{
private:
	Event				m_eOnClick;
	CMaterial*			m_pMtrl;
	BUTTON_STATE		m_eState;

public:
	virtual void Awake();
	virtual int Update();

public:
	void Init();
	void SetOnClick(Event _Function);
	void OnClick();
	void SetState(const BUTTON_STATE& _eState);
	bool IsMouseOn(const Vec2& _vPos) { return GetGameObject()->IsMouseOn(_vPos); }

public:
	CLONE(CButtonScript);
	virtual void Save(FILE * _pFile);
	virtual void Load(FILE* _pFile);

public:
	CButtonScript();
	virtual ~CButtonScript();
};

