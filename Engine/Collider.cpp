#include "Collider.h"

#include "Transform.h"

#include "Device.h"

#include "GameObject.h"
#include "Script.h"

UINT CCollider::g_iColID = 0;

CCollider::CCollider()
	: m_iColID(g_iColID++)	
	, m_pMesh(NULL)
	, m_pMtrl(NULL)
	, m_iCollision(0)
	, m_bScale(true)	
	, m_eType(COLLIDER_TYPE::END)
{
	m_vOffsetScale = Vec3(1.01f, 1.01f, 1.01f);
}

CCollider::CCollider(const CCollider & _other)
	: m_iColID(g_iColID++)
	, m_pMesh(_other.m_pMesh)
	, m_pMtrl(_other.m_pMtrl)
	, m_iCollision(_other.m_iCollision)
	, m_vObjPos(_other.m_vObjPos)
	, m_vObjScale(_other.m_vObjScale)
	, m_vObjRot(_other.m_vObjRot)
	, m_vOffsetPos(_other.m_vOffsetPos)
	, m_vOffsetScale(_other.m_vOffsetScale)
	, m_matColliderWorld(_other.m_matColliderWorld)
	, m_bScale(_other.m_bScale)
{}

CCollider::~CCollider()
{
}

void CCollider::OnCollisionEnter(CCollider * _pOther)
{	
	map<SCRIPT_ID, CScript*>& listScript = GetGameObject()->GetScriptList();
	map<SCRIPT_ID, CScript*>::iterator iter = listScript.begin();
	for (; iter != listScript.end(); ++iter)
	{
		iter->second->OnCollisionEnter(_pOther);
	}
}

void CCollider::OnCollision(CCollider * _pOther)
{
	m_iCollision = 1;

	map<SCRIPT_ID, CScript*>& listScript = GetGameObject()->GetScriptList();
	map<SCRIPT_ID, CScript*>::iterator iter = listScript.begin();
	for (; iter != listScript.end(); ++iter)
	{
		iter->second->OnCollision(_pOther);
	}
}

void CCollider::OnCollisionExit(CCollider * _pOther)
{
	m_iCollision = 0;

	map<SCRIPT_ID, CScript*>& listScript = GetGameObject()->GetScriptList();
	map<SCRIPT_ID, CScript*>::iterator iter = listScript.begin();
	for (; iter != listScript.end(); ++iter)
	{
		iter->second->OnCollisionExit(_pOther);
	}
}

int CCollider::FinalUpdate()
{
	m_vObjPos = Transform()->GetWorldPos();
	m_vObjScale = Transform()->GetWorldScale();
	m_vObjRot = Transform()->GetLocalRot();

	// Collider 크 * 자(x) * 이동 * (행렬)
	if (m_bScale)
	{		
		m_matColliderWorld = XMMatrixScaling(m_vOffsetScale.x, m_vOffsetScale.y, m_vOffsetScale.z);
	}	
	else
	{
		Vec3 ObjScale = Transform()->GetWorldScale();
		ObjScale = m_vOffsetScale / ObjScale;
		m_matColliderWorld = XMMatrixScaling(ObjScale.x, ObjScale.y, ObjScale.z);
	}	

	m_matColliderWorld *= XMMatrixTranslation(m_vOffsetPos.x, m_vOffsetPos.y, m_vOffsetPos.z);
	m_matColliderWorld *= Transform()->GetWorldMat();

	return 0;
}

void CCollider::SetColType(const COLLIDER_TYPE & _Type)
{
	m_eType = _Type;

	if (COLLIDER_TYPE::SPHERE == m_eType)
		m_pMesh = CResMgr::GetInst()->FindMesh(L"SphereMesh");
	else if (COLLIDER_TYPE::CUBE == m_eType || COLLIDER_TYPE::OBB == m_eType)
		m_pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(L"ColliderBoxMesh");
}

void CCollider::UpdateData()
{
	g_Transform.matWorld = m_matColliderWorld;
	g_Transform.matWorld = XMMatrixTranspose(g_Transform.matWorld);

	const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"Transform");

	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &g_Transform, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);


	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
}

Vec3 CCollider::GetWorldPos()
{
	Vec3 vWorldPos = XMVector3TransformCoord(m_vOffsetPos.Convert(), Transform()->GetWorldMat());
	return vWorldPos;
}

void CCollider::Save(FILE * _pFile)
{
	SaveResourceKey(m_pMesh.GetTarget(), _pFile);
	SaveResourceKey(m_pMtrl.GetTarget(), _pFile);

	fwrite(&m_vObjPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vObjScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vObjRot, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fwrite(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
	fwrite(&m_bScale, 1, 1, _pFile);
	fwrite(&m_eType, sizeof(COLLIDER_TYPE), 1, _pFile);
}

void CCollider::Load(FILE * _pFile)
{
	wstring strKey;
	strKey = LoadResourceKey(_pFile);
	if (!strKey.empty())
		m_pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(strKey);
	
	strKey = LoadResourceKey(_pFile);
	if (!strKey.empty())
		m_pMtrl = CResMgr::GetInst()->FindMaterial(strKey);

	fread(&m_vObjPos, sizeof(Vec3), 1, _pFile);
	fread(&m_vObjScale, sizeof(Vec3), 1, _pFile);
	fread(&m_vObjRot, sizeof(Vec3), 1, _pFile);
	fread(&m_vOffsetPos, sizeof(Vec3), 1, _pFile);
	fread(&m_vOffsetScale, sizeof(Vec3), 1, _pFile);
	fread(&m_bScale, 1, 1, _pFile);
	fread(&m_eType, sizeof(COLLIDER_TYPE), 1, _pFile);
}
