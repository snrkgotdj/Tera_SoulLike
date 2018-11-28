#pragma once
#include "Script.h"
class CMoveScript :
	public CScript
{

public:
	virtual void Awake();
	virtual int Update();
	virtual int DelayUpdate();

public:
	CLONE(CMoveScript);
	virtual void Save(FILE * _pFile);
	virtual void Load(FILE* _pFile);

public:
	CMoveScript();
	virtual ~CMoveScript();
};

