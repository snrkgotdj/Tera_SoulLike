#include "Collider_2D.h"

#include "ResMgr.h"
#include "Material.h"
#include "Mesh.h"
#include "Device.h"

#include "Shader.h"

CCollider_2D::CCollider_2D()
{
}

CCollider_2D::~CCollider_2D()
{
}

void CCollider_2D::Awake()
{
	m_pMtrl = (CMaterial*)CResMgr::GetInst()->FindMaterial(L"ColliderMtrl");
	m_pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(L"ColliderRectMesh");
}

void CCollider_2D::Render()
{
	if (m_pMtrl == NULL || m_pMesh == NULL)
		return;
	
	// Transform Á¤º¸ update
	UpdateData();
		
	m_pMtrl->SetData(SHADER_PARAM::SP_INT_0, &m_iCollision);
	m_pMtrl->UpdateData();

	m_pMesh->SetLayout(m_pMtrl->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();

	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_NONE);

	m_pMesh->Render();
}

void CCollider_2D::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::COLLIDER_2D;
	fwrite(&iType, sizeof(UINT), 1, _pFile);
		
	CCollider::Save(_pFile);
}

void CCollider_2D::Load(FILE * _pFile)
{
	// Collider 2D

	// Collider
	CCollider::Load(_pFile);
}