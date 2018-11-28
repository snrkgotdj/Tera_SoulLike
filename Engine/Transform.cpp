#include "Transform.h"

#include "Device.h"
#include "KeyMgr.h"
#include "TimeMgr.h"

#include "GameObject.h"
#include "Animator_3D.h"

tTransform	g_Transform;

Vec3 g_vRight = Vec3(1.f, 0.f, 0.f);
Vec3 g_vUp = Vec3(0.f, 1.f, 0.f);
Vec3 g_vFront = Vec3(0.f, 0.f, 1.f);


CTransform::CTransform()
	: m_vPos(Vec3(0.f, 0.f, 0.f))
	, m_vScale(Vec3(1.f, 1.f, 1.f))
	, m_vRot(Vec3(0.f, 0.f, 0.f))
	, m_vDir{Vec3(1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 1.f)}
	, m_bCull(false)
	, m_bBillBoard(false)
	, m_fCulRadius(100)
	, m_bIsParts(false)
	, m_iRimNum(0)
{
	m_matWorld = XMMatrixIdentity();
}

CTransform::~CTransform()
{
}

int CTransform::Update()
{	
	//if (Animator_3D())
	//	SetAnimationFinalPos();

	return 0;
}

int CTransform::LateUpdate()
{
	return 0;
}

int CTransform::FinalUpdate()
{
	XMMATRIX matScale = XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);

	XMMATRIX matTransform = XMMatrixTranslation(m_vPos.x, m_vPos.y, m_vPos.z);

	XMMATRIX matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	matRot *= XMMatrixRotationZ(m_vRot.z);

	if (m_bBillBoard)
	{
		m_matWorld = m_matBillBoard;
	}
	else
	{
		m_matWorld = matScale * matRot * matTransform;
	}

	if (m_bIsParts == true)
	{
		m_matWorld *= m_matBone;
	}

	
	CGameObject* pParentObj = GetParentObject();
	if (NULL != pParentObj)
	{
		m_matWorld *= pParentObj->Transform()->GetWorldMat();
	}

	m_vDir[(UINT)DIR_TYPE::RIGHT]	= Vec3(1.f, 0.f, 0.f);
	m_vDir[(UINT)DIR_TYPE::UP]		= Vec3(0.f, 1.f, 0.f);
	m_vDir[(UINT)DIR_TYPE::FRONT]	= Vec3(0.f, 0.f, 1.f);

	m_vDir[(UINT)DIR_TYPE::RIGHT] = XMVector3TransformNormal(g_vRight.Convert(), m_matWorld);
	m_vDir[(UINT)DIR_TYPE::RIGHT].Normalize();

	m_vDir[(UINT)DIR_TYPE::UP] = XMVector3TransformNormal(g_vUp.Convert(), m_matWorld);
	m_vDir[(UINT)DIR_TYPE::UP].Normalize();

	m_vDir[(UINT)DIR_TYPE::FRONT] = XMVector3TransformNormal(g_vFront.Convert(), m_matWorld);
	m_vDir[(UINT)DIR_TYPE::FRONT].Normalize();

	return 0;
}

void CTransform::UpdateData()
{
	g_Transform.matWorld = m_matWorld;
	g_Transform.matWorld = XMMatrixTranspose(g_Transform.matWorld);
	
	g_Transform.matWV = g_Transform.matView * g_Transform.matWorld;
	g_Transform.matWVP = g_Transform.matProj * g_Transform.matView * g_Transform.matWorld;
	g_Transform.matPrevWVP = m_matPrevWVP;
	g_Transform.g_fDelta.z = (float)m_iRimNum;
	const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"Transform");
	
	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &g_Transform, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->HSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->DSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->GSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->CSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	//이전 WVP저장
	m_matPrevWVP = g_Transform.matWVP;
}

void CTransform::UpdateData(Matrix _m_matWorld)
{
	g_Transform.matWorld = _m_matWorld;
	g_Transform.matWorld = XMMatrixTranspose(g_Transform.matWorld);

	g_Transform.matWV = g_Transform.matView * g_Transform.matWorld;
	g_Transform.matWVP = g_Transform.matProj * g_Transform.matView * g_Transform.matWorld;
	g_Transform.matPrevWVP = m_matPrevWVP;

	const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"Transform");

	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &g_Transform, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->HSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->DSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->GSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->CSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	//이전 WVP저장
	m_matPrevWVP = g_Transform.matWV;
}


Vec3 CTransform::GetWorldPos() const
{
	CGameObject* pParentObj = GetParentObject();
	Vec3 vWorldPos = m_vPos;
	if (NULL != pParentObj)
	{	
		vWorldPos = XMVector3TransformCoord(m_vPos.Convert(), pParentObj->Transform()->GetWorldMat());		
	}

	return vWorldPos;
}

Vec3 CTransform::GetWorldScale()
{
	CGameObject* pParent = GetParentObject();
	Vec3 vWorldScale = m_vScale;
	while (pParent)
	{
		vWorldScale *= pParent->Transform()->GetLocalScale();

		pParent = pParent->GetParentObj();
	}

	return vWorldScale;
}

void CTransform::SetLocalRotMat(const Matrix & _mat)
{
	m_vRot = DecomposeRotMat(_mat);
}

void CTransform::RecalculationBeChild()
{
	Matrix matParentWorld = GetParentObject()->Transform()->GetWorldMat();
	Matrix matParentInv = XMMatrixInverse(NULL, matParentWorld);
	Matrix matWorld = m_matWorld * matParentInv;

	XMVECTOR vScale;
	XMVECTOR vRot;
	XMVECTOR vPos;

	XMMatrixDecompose(&vScale, &vRot, &vPos, matWorld);
	Matrix matRot = XMMatrixRotationQuaternion(vRot);

	m_vScale = vScale;
	m_vPos = vPos;
	m_vRot = DecomposeRotMat(matRot);
}

void CTransform::RecalculationBeParent()
{
	XMVECTOR vScale;
	XMVECTOR vRot;
	XMVECTOR vPos;

	XMMatrixDecompose(&vScale, &vRot, &vPos, m_matWorld);
	Matrix matRot = XMMatrixRotationQuaternion(vRot);

	m_vScale = vScale;
	m_vPos = vPos;
	m_vRot = DecomposeRotMat(matRot);
}

void CTransform::SetAnimationFinalPos()
{
	if (false == Animator_3D()->IsAnimFinish())
		return;

	const Matrix& vStartMat = Animator_3D()->GetPrevSpineMat();
	const Matrix& vFinishPos = Animator_3D()->GetFinalBoneMatrix(4);

	Vec3 StartPos = Vec3(vStartMat._41, vStartMat._42, vStartMat._43);
	Vec3 FinishPos = Vec3(vFinishPos._41, vFinishPos._42, vFinishPos._43);

	FinishPos *= m_vScale;
	StartPos *= m_vScale;

	Vec3 vDist = FinishPos - StartPos;
	m_vPos.x += vDist.x;
	m_vPos.z += vDist.z;

}

bool closeEnough(const float& a, const float& b
	, const float& epsilon = std::numeric_limits<float>::epsilon())
{
	return (epsilon > std::abs(a - b));
}

Vec3 CTransform::DecomposeRotMat(const Matrix & _matRot)
{
	// _mat 을 분해 후 다시 행렬 만들기	
	Vec4 vMat[4];
	XMStoreFloat4(&vMat[0], _matRot.r[0]);
	XMStoreFloat4(&vMat[1], _matRot.r[1]);
	XMStoreFloat4(&vMat[2], _matRot.r[2]);
	XMStoreFloat4(&vMat[3], _matRot.r[3]);

	Vec3 vNewRot;
	if (closeEnough(vMat[0].z, -1.0f)) {
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = XM_PI / 2;
		float z = x + atan2(vMat[1].x, vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else if (closeEnough(vMat[0].z, 1.0f)) {
		float x = 0;
		float y = -XM_PI / 2;
		float z = -x + atan2(-vMat[1].x, -vMat[2].x);
		vNewRot = Vec3{ x, y, z };
	}
	else { //two solutions exist
		float y1 = -asin(vMat[0].z);
		float y2 = XM_PI - y1;

		float x1 = atan2f(vMat[1].z / cos(y1), vMat[2].z / cos(y1));
		float x2 = atan2f(vMat[1].z / cos(y2), vMat[2].z / cos(y2));

		float z1 = atan2f(vMat[0].y / cos(y1), vMat[0].x / cos(y1));
		float z2 = atan2f(vMat[0].y / cos(y2), vMat[0].x / cos(y2));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2)))
		{
			vNewRot = Vec3{ x1, y1, z1 };
		}
		else {
			vNewRot = Vec3{ x2, y2, z2 };
		}
	}
	return vNewRot;
}

void CTransform::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::TRANSFORM;

	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_vPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vRot, sizeof(Vec3), 1, _pFile);	
	fwrite(&m_bCull, sizeof(bool), 1, _pFile);
	fwrite(&m_bBillBoard, sizeof(bool), 1, _pFile);
	fwrite(&m_fCulRadius, sizeof(float), 1, _pFile);
}

void CTransform::Load(FILE * _pFile)
{
	fread(&m_vPos, sizeof(Vec3), 1, _pFile);
	fread(&m_vScale, sizeof(Vec3), 1, _pFile);
	fread(&m_vRot, sizeof(Vec3), 1, _pFile);
	fread(&m_bCull, sizeof(bool), 1, _pFile);
	fread(&m_bBillBoard, sizeof(bool), 1, _pFile);
	fread(&m_fCulRadius, sizeof(float), 1, _pFile);
}
