#include "Collider_3D.h"

#include "ResMgr.h"
#include "Material.h"
#include "Mesh.h"
#include "Device.h"

#include "Shader.h"

CCollider_3D::CCollider_3D()
{
	m_pMtrl = (CMaterial*)CResMgr::GetInst()->FindMaterial(L"Mtrl_Collider");
}


CCollider_3D::~CCollider_3D()
{

}


void CCollider_3D::Awake()
{
}

void CCollider_3D::Render()
{
	if (m_pMtrl == NULL || m_pMesh == NULL)
		return;

	// Transform Á¤º¸ update
	UpdateData();

	m_pMtrl->SetData(SHADER_PARAM::SP_INT_0, &m_iCollision);
	m_pMtrl->UpdateData();

	m_pMesh->SetLayout(m_pMtrl->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();

	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_WIREFRAME);

	m_pMesh->Render();
}

void CCollider_3D::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::COLLIDER_3D;
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	CCollider::Save(_pFile);
}

void CCollider_3D::Load(FILE * _pFile)
{
	// Collider 2D

	// Collider
	CCollider::Load(_pFile);
}