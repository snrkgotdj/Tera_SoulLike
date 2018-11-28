#pragma once
#include "Script.h"


class CGridScript :
	public CScript
{
private:
	CGameObject*  m_pToolCam;
	int			  m_iMinStep;
	int			  m_iMaxStep;
	float		  m_fBoldRatio;


private:
	void FindToolCam();
	void CalGridStep();

public:
	virtual int Update();
	CLONE(CGridScript);

public:
	CGridScript();
	virtual ~CGridScript();
};

