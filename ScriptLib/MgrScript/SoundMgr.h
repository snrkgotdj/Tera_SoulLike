#pragma once
#include "STScript.h"

class CSoundMgr :
	public CSTScript<CSoundMgr>
{
public:
	virtual int Update() { return 0; }

public:
	void Init();

public:
	CLONE(CSoundMgr);

public:
	CSoundMgr();
	virtual ~CSoundMgr();
};

