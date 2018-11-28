#pragma once

#include "global.h"

enum PLANE_TYPE
{
	PT_NEAR,
	PT_FAR,
	PT_LEFT,
	PT_RIGHT,
	PT_TOP,
	PT_BOT,
	PT_END
};

class CCamera;

class CFrustum
{
private:
	CCamera*	m_pCam;
	Vec3		m_arrPos[8];
	XMVECTOR	m_vPlane[6];

public:
	void Init();
	int FinalUpdate();

	bool FrunstumCheckPoint(Vec3 _vWorldPos);
	bool FrunstumCheckSphere(Vec3 _vWorldCenter, float _fRadius);

public:
	CFrustum(CCamera* _pCam);
	~CFrustum();
};

