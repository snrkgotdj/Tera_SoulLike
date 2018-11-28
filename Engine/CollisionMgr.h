#pragma once

#include "global.h"

union COL_ID
{
	struct
	{
		UINT iLeftID;
		UINT iRightID;
	};
	unsigned long long llID;
};

class CLayer;
class CCollider;

class CCollisionMgr
{
	SINGLE(CCollisionMgr)

private:
	UINT					m_arrColMat[MAX_LAYER];
	map<long long, bool>	m_mapColID;

public:
	int Init();
	int Update();

private:
	void CollsionLayer(CLayer* _pLeft, CLayer* _pRight);
	bool IsCollision(CCollider* _pLeftCol, CCollider* _pRightCol);
	void GetProjAxis(const Matrix& _matWorld, Vec3* _vProj, Vec3* _vSource);
	void GetProjAxis3D(const Matrix& _matWorld, Vec3* _vProj, float* _vRadius);
	
	bool IsCollisionAABB(CCollider* _pLeftCol, CCollider* _pRightCol);
	bool IsCollisionSphere(CCollider* _pLeftCol, CCollider* _pRightCol);
	bool IsCollisionOBB(CCollider* _pLeftCol, CCollider* _pRightCol);
	bool IsCollisionOBBAndSphere(CCollider* _pLeftCol, CCollider* _pRightCol, bool _bLeftOBB);

public:
	int CheckCollisionLayer(const wstring& _strLeft, const wstring& _strRight);

};

