#pragma once
#include "Script.h"

class CTerrainScript :
	public CScript
{
public:
	virtual int Update();
	virtual int DelayUpdate();

public:
	CLONE(CTerrainScript)

public:
	CTerrainScript();
	virtual ~CTerrainScript();
};

