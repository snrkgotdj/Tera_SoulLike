#pragma once
#include "ParentScript\PlayerScript.h"

class CWarriorScript :
	public CPlayerScript
{	
public:
	virtual void Start();

public:
	CLONE(CWarriorScript);

public:
	CWarriorScript();
	~CWarriorScript();
};

