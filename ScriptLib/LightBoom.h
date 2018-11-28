#pragma once
#include "Script.h"
class CLightBoom :
	public CScript
{

public:
	virtual int Update();

	CLONE(CLightBoom)

public:
	CLightBoom();
	virtual ~CLightBoom();
};

