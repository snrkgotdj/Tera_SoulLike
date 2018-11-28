#pragma once
#include "STScript.h"

class CButtonScript;

class CUIMgrScript :
	public CSTScript<CUIMgrScript>
{
private:
	list<CButtonScript*>	m_listButton;

public:
	int Update();

private:
	int ButtonUpdate();

public:
	void AddButton(CButtonScript* _pButton);

public:
	CLONE(CUIMgrScript);

public:
	CUIMgrScript();
	virtual ~CUIMgrScript();
};

