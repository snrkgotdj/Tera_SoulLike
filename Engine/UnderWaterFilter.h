#pragma once
#include "CamFilter.h"
class CUnderWaterFilter :
	public CCamFilter
{
public:
	virtual int FinalUpdate();

public:
	CUnderWaterFilter();
	virtual ~CUnderWaterFilter();
};

