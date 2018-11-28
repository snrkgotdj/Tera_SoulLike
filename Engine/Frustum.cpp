#include "Frustum.h"

#include "Camera.h"

CFrustum::CFrustum(CCamera * _pCam)
	: m_arrPos{}
	, m_pCam(_pCam)
{
}

CFrustum::~CFrustum()
{
}

void CFrustum::Init()
{
	m_arrPos[0] = Vec3(-1.f, 1.f, 0.f);
	m_arrPos[1] = Vec3(1.f, 1.f, 0.f);
	m_arrPos[2] = Vec3(1.f, -1.f, 0.f);
	m_arrPos[3] = Vec3(-1.f, -1.f, 0.f);

	m_arrPos[4] = Vec3(-1.f, 1.f, 1.f);
	m_arrPos[5] = Vec3(1.f, 1.f, 1.f);
	m_arrPos[6] = Vec3(1.f, -1.f, 1.f);
	m_arrPos[7] = Vec3(-1.f, -1.f, 1.f);
}

int CFrustum::FinalUpdate()
{
	const Matrix& matView = m_pCam->GetViewMat();
	const Matrix& matProj = m_pCam->GetProjMat();

	Matrix matInv = matView * matProj;
	matInv = XMMatrixInverse(NULL, matInv);

	Vec3 vWorldPos[8] = {};

	for (UINT i = 0; i < 8; ++i)
	{
		vWorldPos[i] = XMVector3TransformCoord(m_arrPos[i].Convert(), matInv);
	}

	m_vPlane[PT_NEAR] = XMPlaneFromPoints(vWorldPos[0].Convert(), vWorldPos[1].Convert(), vWorldPos[2].Convert());
	m_vPlane[PT_FAR] = XMPlaneFromPoints(vWorldPos[6].Convert(), vWorldPos[5].Convert(), vWorldPos[4].Convert());
	m_vPlane[PT_LEFT] = XMPlaneFromPoints(vWorldPos[4].Convert(), vWorldPos[0].Convert(), vWorldPos[7].Convert());
	m_vPlane[PT_RIGHT] = XMPlaneFromPoints(vWorldPos[1].Convert(), vWorldPos[5].Convert(), vWorldPos[6].Convert());
	m_vPlane[PT_TOP] = XMPlaneFromPoints(vWorldPos[0].Convert(), vWorldPos[4].Convert(), vWorldPos[5].Convert());
	m_vPlane[PT_BOT] = XMPlaneFromPoints(vWorldPos[2].Convert(), vWorldPos[6].Convert(), vWorldPos[7].Convert());

	//bool bTrue = FrunstumCheckPoint(Vec3(0.f, 0.f, 15000.f));
	bool bTrue = FrunstumCheckSphere(Vec3(0.f, 0.f, 5000.f), 1.f);

	int a = 10;

	return 0;
}

bool CFrustum::FrunstumCheckPoint(Vec3 _vWorldPos)
{
	XMVECTOR vRet;
	for (UINT i = 0; i < (UINT)PLANE_TYPE::PT_END; ++i)
	{
		vRet = XMPlaneDotCoord(m_vPlane[i], _vWorldPos.Convert());
		if (0.f < vRet.vector4_f32[0])
		{
			return false;
		}
	}

	return true;
}

bool CFrustum::FrunstumCheckSphere(Vec3 _vWorldCenter, float _fRadius)
{
	XMVECTOR vRet;
	for (UINT i = 0; i < (UINT)PT_END; ++i)
	{
		vRet = XMPlaneDotCoord(m_vPlane[i], _vWorldCenter.Convert());

		if (_fRadius < vRet.vector4_f32[0])
		{
			return false;
		}
	}

	return true;
}