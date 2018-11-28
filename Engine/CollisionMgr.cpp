#include "CollisionMgr.h"

#include "Scene.h"
#include "SceneMgr.h"
#include "Layer.h"

#include "GameObject.h"
#include "Collider.h"

CCollisionMgr::CCollisionMgr()
	: m_arrColMat{}
{
}

CCollisionMgr::~CCollisionMgr()
{
}

int CCollisionMgr::Init()
{
	CheckCollisionLayer(L"Default", L"Monster");
	CheckCollisionLayer(L"Default", L"Player");
	CheckCollisionLayer(L"Monster", L"Monster");
	CheckCollisionLayer(L"Monster", L"Rock");
	CheckCollisionLayer(L"Player", L"Rock");
	CheckCollisionLayer(L"Player", L"Monster");

	return 0;
}

int CCollisionMgr::Update()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL == vecLayer[i])
			continue;

		for (UINT j = i; j < MAX_LAYER; ++j)
		{
			if ((m_arrColMat[i] >> j) & 1)
			{
				// 레이어가 제거 됬을 경우 체크해제
				if (NULL == vecLayer[j])
				{
					//CheckCollisionLayer(vecLayer[i]->GetLayerName(), vecLayer[j]->GetLayerName());
					continue;
				}

				CollsionLayer(vecLayer[i], vecLayer[j]);
			}
		}
	}

	return 0;
}

int CCollisionMgr::CheckCollisionLayer(const wstring & _strLeft, const wstring & _strRight)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

	CLayer* pLeft = pCurScene->FindLayer(_strLeft);
	CLayer* pRight = pCurScene->FindLayer(_strRight);

	if (NULL == pLeft || NULL == pRight)
		return RET_FAILED;

	UINT iLeftIdx = pLeft->GetLayerIdx();
	UINT iRightIdx = pRight->GetLayerIdx();

	if (iRightIdx < iLeftIdx)
	{
		UINT iTemp = iLeftIdx;
		iLeftIdx = iRightIdx;
		iRightIdx = iTemp;
	}

	if (m_arrColMat[iLeftIdx] & (1 << iRightIdx))
		m_arrColMat[iLeftIdx] &= ~(1 << iRightIdx);	
	else
		m_arrColMat[iLeftIdx] |= (1 << iRightIdx);	

	return RET_SUCCESS;
}

void CCollisionMgr::CollsionLayer(CLayer * _pLeft, CLayer * _pRight)
{
	list<CGameObject*>& listLeft = _pLeft->GetObjList();
	list<CGameObject*>& listRight = _pRight->GetObjList();

	list<CGameObject*>::iterator iterLeft = listLeft.begin();
	CCollider* pLeft = NULL;
	CCollider* pRight = NULL;

	for (; iterLeft != listLeft.end(); ++iterLeft)
	{
		pLeft = (*iterLeft)->Collider();
		if (NULL == pLeft)
			continue;

		list<CGameObject*>::iterator iterRight = listRight.begin();
		for (; iterRight != listRight.end(); ++iterRight)
		{
			pRight = (*iterRight)->Collider();
			if (NULL == pRight || (pLeft->Is2DCollider() != pRight->Is2DCollider()) || pRight == pLeft)
				continue;

			COL_ID id;
			id.iLeftID = pLeft->GetColID();
			id.iRightID = pRight->GetColID();

			map<long long, bool>::iterator iterID = m_mapColID.find(id.llID);
			bool bPrev = false;
			if (iterID != m_mapColID.end())
				bPrev = iterID->second;

			bool isCollision = false;
			if( pLeft->Is2DCollider() == true)
				isCollision = IsCollision(pLeft, pRight);
			else
			{
				if (pLeft->GetColType() == COLLIDER_TYPE::SPHERE && pRight->GetColType() == COLLIDER_TYPE::SPHERE)
				{
					isCollision = IsCollisionSphere(pLeft, pRight);
				}
				else if(pLeft->GetColType() == COLLIDER_TYPE::OBB && pRight->GetColType() == COLLIDER_TYPE::OBB)
				{
					isCollision = IsCollisionOBB(pLeft, pRight);
				}
				else if (pLeft->GetColType() == COLLIDER_TYPE::OBB && pRight->GetColType() == COLLIDER_TYPE::SPHERE)
				{
					isCollision = IsCollisionOBBAndSphere(pLeft, pRight, true);
				}
				else if (pLeft->GetColType() == COLLIDER_TYPE::SPHERE && pRight->GetColType() == COLLIDER_TYPE::OBB)
				{
					isCollision = IsCollisionOBBAndSphere(pLeft, pRight, false);
				}
			}

			if (isCollision)
			{
				// 충돌 했다.
				if (bPrev)
				{
					// 이전에도 충돌했었다.
					pLeft->OnCollision(pRight);
					pRight->OnCollision(pLeft);
				}
				else
				{
					// 최초 충돌 시점
					pLeft->OnCollisionEnter(pRight);
					pRight->OnCollisionEnter(pLeft);

					if (iterID == m_mapColID.end())
					{
						m_mapColID.insert(make_pair(id.llID, true));
					}
					else
					{
						iterID->second = true;
					}
				}
			}
			else
			{
				// 충돌하지 않았다.
				if (bPrev)
				{
					// 충돌이 막 벗어난 시점이다.
					pLeft->OnCollisionExit(pRight);
					pRight->OnCollisionExit(pLeft);
					iterID->second = false;
				}
			}
		}
	}
}

bool CCollisionMgr::IsCollision(CCollider * _pLeftCol, CCollider * _pRightCol)
{
	Vec3 vProj[4] = {};
	Vec3 vSouce[4] = {};
	Vec3 vCenter = _pRightCol->GetWorldPos() - _pLeftCol->GetWorldPos();

	GetProjAxis(_pLeftCol->GetWorldMat(), vProj, vSouce);
	GetProjAxis(_pRightCol->GetWorldMat(), vProj + 2, vSouce + 2);

	for (int i = 0; i < 4; ++i)
	{
		float fDist = 0.f;
		for (int j = 0; j < 4; ++j)
		{
			fDist += abs(vSouce[j].Dot(vProj[i]));
		}
		fDist /= 2.f;

		float fCenter = abs(vCenter.Dot(vProj[i]));

		if (fCenter > fDist)
			return false;
	}
	return true;
}

void CCollisionMgr::GetProjAxis(const Matrix & _matWorld, Vec3 * _vProj, Vec3 * _vSource)
{
	// 1 ----- 2
	// |       |
	// |       |
	// 0 ----- 3
	Vec3 vLocal[4] = { Vec3(-0.5f, -0.5f, 0.f), Vec3(-0.5f, 0.5f, 0.f)
		, Vec3(0.5f, 0.5f, 0.f), Vec3(0.5f, -0.5f, 0.f) };

	Vec3 vOut1 = XMVector3TransformCoord(vLocal[2].Convert(), _matWorld);
	Vec3 vOut2 = XMVector3TransformCoord(vLocal[1].Convert(), _matWorld);

	_vSource[0] = vOut1 - vOut2;
	_vSource[0].z = 0.f;
	_vProj[0] = _vSource[0];
	_vProj[0].Normalize();

	vOut1 = XMVector3TransformCoord(vLocal[0].Convert(), _matWorld);
	vOut2 = XMVector3TransformCoord(vLocal[1].Convert(), _matWorld);

	_vSource[1] = vOut1 - vOut2;
	_vSource[1].z = 0.f;
	_vProj[1] = _vSource[1];
	_vProj[1].Normalize();
}

void CCollisionMgr::GetProjAxis3D(const Matrix & _matWorld, Vec3 * _vProj, float * _vRadius)
{
	Vec3 vLocal[4] =
	{
		Vec3(-0.5f, 0.5f, 0.5f),	// 0
		Vec3(0.5f, 0.5f, 0.5f),		// 1
		Vec3(0.5f, 0.5f, -0.5f),	// 2
		Vec3(0.5f, -0.5f, 0.5f)		// 3
	};

	Vec3 vWorldPos[4] = {};
	for (int i = 0; i < 4; ++i)
	{
		vWorldPos[i] = XMVector3TransformCoord(vLocal[i].Convert(), _matWorld);
	}

	Vec3 vProj = vWorldPos[0] - vWorldPos[1];
	_vProj[0] = vProj;

	vProj = vWorldPos[3] - vWorldPos[1];
	_vProj[1] = vProj;

	vProj = vWorldPos[2] - vWorldPos[1];
	_vProj[2] = vProj;

	for (int i = 0; i < 3; ++i)
	{
		_vRadius[i] = _vProj[i].Distance() / 2.f;

		if (0 != _vProj[i].Distance())
			_vProj[i].Normalize();
	}
}

bool CCollisionMgr::IsCollisionAABB(CCollider * _pLeftCol, CCollider * _pRightCol)
{
	return false;
}

bool CCollisionMgr::IsCollisionSphere(CCollider * _pLeftCol, CCollider * _pRightCol)
{
	Vec3 vLeftScale = _pLeftCol->GetWorldScale();
	Vec3 vLeftPos = _pLeftCol->GetWorldPos();

	Vec3 vRightScale = _pRightCol->GetWorldScale();
	Vec3 vRightPos = _pRightCol->GetWorldPos();

	Vec3 vDist = vLeftPos - vRightPos;

	float fDist = vDist.Distance();

	if (fDist <= vLeftScale.x + vRightScale.x)
	{
		return true;
	}

	return false;
}

bool CCollisionMgr::IsCollisionOBB(CCollider * _pLeftCol, CCollider * _pRightCol)
{
	Vec3 vLeftPos = _pLeftCol->GetWorldPos();
	Vec3 vRightPos = _pRightCol->GetWorldPos();

	Vec3 vDirPos = vLeftPos - vRightPos;		// 두 중심의 점간의 방향 벡터(D)
	Vec3 vDotDirPos = {};						// 위의 벡터를 임의의 축에 투영한 벡터(AD)
	float matRot[3][3] = {};					// 두 정육면체의 평변의 법선 벡터끼리의 내적된 값(C)
	float absMatRot[3][3] = {};					// 위의 절대값(absC)
	float fR = 0.f;								// fR : 두 정육면체의 중심을 임의의 축에 투영한 길이, Left, Right : 각 정육면체를 임의의 축에 투영한 길이 (R, R0, R1)
	float fRLeft = 0.f;
	float fRRight = 0.f;			

	Vec3 arrLeftAxis[3] = {};		// 0 : X, 1 : Y, 2 : Z 축방향
	float arrLeftRadius[3] = {};

	Vec3 arrRightAxis[3] = {};
	float arrRightRadius[3] = {};

	// 각 정육면체의 3개의 축을 구해야 한다.
	GetProjAxis3D(_pLeftCol->GetWorldMat(), arrLeftAxis, arrLeftRadius);
	GetProjAxis3D(_pRightCol->GetWorldMat(), arrRightAxis, arrRightRadius);

	// 조건문 검사
	// Left 0
	matRot[0][0] = arrLeftAxis[0].Dot(arrRightAxis[0]);
	matRot[0][1] = arrLeftAxis[0].Dot(arrRightAxis[1]);
	matRot[0][2] = arrLeftAxis[0].Dot(arrRightAxis[2]);
	vDotDirPos[0] = arrLeftAxis[0].Dot(vDirPos);
	absMatRot[0][0] = abs(matRot[0][0]);
	absMatRot[0][1] = abs(matRot[0][1]);
	absMatRot[0][2] = abs(matRot[0][2]);
	fR = abs(vDotDirPos[0]);
	fRRight = (arrRightRadius[0] * absMatRot[0][0]) + (arrRightRadius[1] * absMatRot[0][1]) + (arrRightRadius[2] * absMatRot[0][2]);
	fRLeft = arrLeftRadius[0];
	if (fR > fRLeft + fRRight)
		return false;

	// Left 1
	matRot[1][0] = arrLeftAxis[1].Dot(arrRightAxis[0]);
	matRot[1][1] = arrLeftAxis[1].Dot(arrRightAxis[1]);
	matRot[1][2] = arrLeftAxis[1].Dot(arrRightAxis[2]);
	vDotDirPos[1] = arrLeftAxis[1].Dot(vDirPos);
	absMatRot[1][0] = abs(matRot[1][0]);
	absMatRot[1][1] = abs(matRot[1][1]);
	absMatRot[1][2] = abs(matRot[1][2]);
	fR = abs(vDotDirPos[1]);
	fRRight = (arrRightRadius[0] * absMatRot[1][0]) + (arrRightRadius[1] * absMatRot[1][1]) + (arrRightRadius[2] * absMatRot[1][2]);
	fRLeft = arrLeftRadius[1];
	if (fR > fRLeft + fRRight)
		return false;

	// Left 2
	matRot[2][0] = arrLeftAxis[2].Dot(arrRightAxis[0]);
	matRot[2][1] = arrLeftAxis[2].Dot(arrRightAxis[1]);
	matRot[2][2] = arrLeftAxis[2].Dot(arrRightAxis[2]);
	vDotDirPos[2] = arrLeftAxis[2].Dot(vDirPos);
	absMatRot[2][0] = abs(matRot[2][0]);
	absMatRot[2][1] = abs(matRot[2][1]);
	absMatRot[2][2] = abs(matRot[2][2]);
	fR = abs(vDotDirPos[2]);
	fRRight = (arrRightRadius[0] * absMatRot[2][0]) + (arrRightRadius[1] * absMatRot[2][1]) + (arrRightRadius[2] * absMatRot[2][2]);
	fRLeft = arrLeftRadius[2];
	if (fR > fRLeft + fRRight)
		return false;

	// Right 0
	fR = abs(arrRightAxis[0].Dot(vDirPos));
	fRLeft = (arrLeftRadius[0] * absMatRot[0][0]) + (arrLeftRadius[1] * absMatRot[1][0]) + (arrLeftRadius[2] * absMatRot[2][0]);
	fRRight = arrRightRadius[0];
	if (fR > fRLeft + fRRight)
		return false;

	// Right 1
	fR = abs(arrRightAxis[1].Dot(vDirPos));
	fRLeft = (arrLeftRadius[0] * absMatRot[0][1]) + (arrLeftRadius[1] * absMatRot[1][1]) + (arrLeftRadius[2] * absMatRot[2][1]);
	fRRight = arrRightRadius[1];
	if (fR > fRLeft + fRRight)
		return false;

	// Right 2
	fR = abs(arrRightAxis[2].Dot(vDirPos));
	fRLeft = (arrLeftRadius[0] * absMatRot[0][2]) + (arrLeftRadius[1] * absMatRot[1][2]) + (arrLeftRadius[2] * absMatRot[2][2]);
	fRRight = arrRightRadius[2];
	if (fR > fRLeft + fRRight)
		return false;

	// Left 0 x Right 0
	fR = abs(vDotDirPos[2] * matRot[1][0] - vDotDirPos[1] * matRot[2][0]);
	fRLeft = arrLeftRadius[1] * absMatRot[2][0] + arrLeftRadius[2] * absMatRot[1][0];
	fRRight = arrRightRadius[1] * absMatRot[0][2] + arrRightRadius[2] * absMatRot[0][1];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 0 x Right 1
	fR = abs(vDotDirPos[2] * matRot[1][1] - vDotDirPos[1] * matRot[2][1]);
	fRLeft = arrLeftRadius[1] * absMatRot[2][1] + arrLeftRadius[2] * absMatRot[1][1];
	fRRight = arrRightRadius[0] * absMatRot[0][2] + arrRightRadius[2] * absMatRot[0][0];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 0 x Right 2
	fR = abs(vDotDirPos[2] * matRot[1][2] - vDotDirPos[1] * matRot[2][2]);
	fRLeft = arrLeftRadius[1] * absMatRot[2][2] + arrLeftRadius[2] * absMatRot[1][2];
	fRRight = arrRightRadius[0] * absMatRot[0][1] + arrRightRadius[1] * absMatRot[0][0];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 1 x Right 0
	fR = abs(vDotDirPos[0] * matRot[2][0] - vDotDirPos[2] * matRot[0][0]);
	fRLeft = arrLeftRadius[0] * absMatRot[2][0] + arrLeftRadius[2] * absMatRot[0][0];
	fRRight = arrRightRadius[1] * absMatRot[1][2] + arrRightRadius[2] * absMatRot[1][1];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 1 x Right 1
	fR = abs(vDotDirPos[0] * matRot[2][1] - vDotDirPos[2] * matRot[0][1]);
	fRLeft = arrLeftRadius[0] * absMatRot[2][1] + arrLeftRadius[2] * absMatRot[0][1];
	fRRight = arrRightRadius[0] * absMatRot[1][2] + arrRightRadius[2] * absMatRot[1][0];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 1 x Right 2
	fR = abs(vDotDirPos[0] * matRot[2][2] - vDotDirPos[2] * matRot[0][2]);
	fRLeft = arrLeftRadius[0] * absMatRot[2][2] + arrLeftRadius[2] * absMatRot[0][2];
	fRRight = arrRightRadius[0] * absMatRot[1][1] + arrRightRadius[1] * absMatRot[1][0];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 2 x Right 0
	fR = abs(vDotDirPos[1] * matRot[0][0] - vDotDirPos[0] * matRot[1][0]);
	fRLeft = arrLeftRadius[0] * absMatRot[1][0] + arrLeftRadius[1] * absMatRot[0][0];
	fRRight = arrRightRadius[1] * absMatRot[2][2] + arrRightRadius[2] * absMatRot[2][1];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 2 x Right 1
	fR = abs(vDotDirPos[1] * matRot[0][1] - vDotDirPos[0] * matRot[1][1]);
	fRLeft = arrLeftRadius[0] * absMatRot[1][1] + arrLeftRadius[1] * absMatRot[0][1];
	fRRight = arrRightRadius[0] * absMatRot[2][2] + arrRightRadius[2] * absMatRot[2][0];
	if (fR > fRLeft + fRRight)
		return false;

	//Left 2 x Right 1\2
	fR = abs(vDotDirPos[1] * matRot[0][2] - vDotDirPos[0] * matRot[1][2]);
	fRLeft = arrLeftRadius[0] * absMatRot[1][2] + arrLeftRadius[1] * absMatRot[0][2];
	fRRight = arrRightRadius[0] * absMatRot[2][1] + arrRightRadius[1] * absMatRot[2][0];
	if (fR > fRLeft + fRRight)
		return false;

	return true;
}

bool CCollisionMgr::IsCollisionOBBAndSphere(CCollider * _pLeftCol, CCollider * _pRightCol, bool _bLeftOBB)
{
	Vec3 vSpherePos;
	Vec3 vRealSpherePos;
	Vec3 vSphereScale;

	Vec3 vOBBPos;
	Vec3 vOBBScale;
	Vec3 vOBBRot;

	Vec3 vOBBPoint;

	if (_bLeftOBB)
	{
		vOBBPos = _pLeftCol->GetWorldPos();
		vOBBScale = _pLeftCol->GetOffsetScale();
		vOBBScale *= _pLeftCol->GetWorldScale();
		vOBBRot = _pLeftCol->GetLocalRot();

		XMMATRIX matRot = XMMatrixRotationX(vOBBRot.x);
		matRot *= XMMatrixRotationY(vOBBRot.y);
		matRot *= XMMatrixRotationZ(vOBBRot.z);
		XMMATRIX matTransform = XMMatrixTranslation(vOBBPos.x, vOBBPos.y, vOBBPos.z);
		XMMATRIX matWorld = matRot * matTransform;

		vSpherePos = XMVector3TransformCoord(_pRightCol->GetWorldPos().Convert(), matWorld);
		vSphereScale = _pRightCol->GetWorldScale();
		vSphereScale *= _pRightCol->GetOffsetScale();
		vRealSpherePos = _pRightCol->GetWorldPos();

		vOBBPos = XMVector3TransformCoord(vOBBPos.Convert(), matWorld);
	}
	else
	{
		vOBBPos = _pRightCol->GetWorldPos();
		vOBBScale = _pLeftCol->GetOffsetScale();
		vOBBScale *= _pLeftCol->GetWorldScale();
		vOBBRot = _pRightCol->GetLocalRot();

		XMMATRIX matRot = XMMatrixRotationX(vOBBRot.x);
		matRot *= XMMatrixRotationY(vOBBRot.y);
		matRot *= XMMatrixRotationZ(vOBBRot.z);
		XMMATRIX matTransform = XMMatrixTranslation(vOBBPos.x, vOBBPos.y, vOBBPos.z);
		XMMATRIX matWorld = matRot* matTransform;

		vSpherePos = XMVector3TransformCoord(_pLeftCol->GetWorldPos().Convert(), matWorld);
		vSphereScale = _pLeftCol->GetWorldScale();
		vSphereScale *= _pLeftCol->GetOffsetScale();
		vRealSpherePos = _pLeftCol->GetWorldPos();

		vOBBPos = XMVector3TransformCoord(vOBBPos.Convert(), matWorld);
	}

	vOBBScale /= 2.f;	

	// X
	if (vSpherePos.x < vOBBPos.x - vOBBScale.x)
		vOBBPoint.x = vOBBPos.x - vOBBScale.x;
	else if (vSpherePos.x > vOBBPos.x + vOBBScale.x)
		vOBBPoint.x = vOBBPos.x + vOBBScale.x;
	else
		vOBBPoint.x = vOBBPos.x;
	
	// Y
	if (vSpherePos.y < vOBBPos.y - vOBBScale.y)
		vOBBPoint.y = vOBBPos.y - vOBBScale.y;
	else if (vSpherePos.y > vOBBPos.y + vOBBScale.y)
		vOBBPoint.y = vOBBPos.y + vOBBScale.y;
	else
		vOBBPoint.y = vOBBPos.y;

	// Z
	if (vSpherePos.z < vOBBPos.z - vOBBScale.z)
		vOBBPoint.z = vOBBPos.z - vOBBScale.z;
	else if (vSpherePos.z > vOBBPos.z + vOBBScale.z)
		vOBBPoint.z = vOBBPos.z + vOBBScale.z;
	else
		vOBBPoint.z = vOBBPos.z;

	Vec3 vDist = vSpherePos - vOBBPoint;
	float fDist = vDist.Distance();
	float fRadius = abs(vSphereScale.x);

	if (fDist > fRadius)
		return false;
	else
		return true;
}