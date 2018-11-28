#include "CamFilter.h"

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"
#include "Device.h"

#include "ResMgr.h"

CCamFilter::CCamFilter()
	: m_pMesh(NULL)
	, m_pMtrl(NULL)
{

}

CCamFilter::~CCamFilter()
{
}


int CCamFilter::FinalUpdate()
{
	m_Trans.FinalUpdate();
	return 0;
}

void CCamFilter::RangeRender()
{
	// 다시 설계
	CMesh* pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(L"LightRange_SphereMesh");
	CMaterial* pMtrl =CResMgr::GetInst()->FindMaterial(L"Mtrl_RangeSphere");

	if (pMesh == NULL || pMtrl == NULL || !pMtrl->GetShader())
		return;

	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_NONE);
	// Transform 정보 update
	m_Trans.UpdateData();
	pMtrl->UpdateData();

	pMesh->SetLayout(pMtrl->GetShader()->GetVSBlob());
	pMesh->UpdateData();
	pMesh->Render();
}

void CCamFilter::Render()
{
	if (m_pMesh == NULL || m_pMtrl == NULL || !m_pMtrl->GetShader())
		return;

	// Transform 정보 update
	m_Trans.UpdateData();
	m_pMtrl->UpdateData();

	m_pMesh->SetLayout(m_pMtrl->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();
	m_pMesh->Render();
}