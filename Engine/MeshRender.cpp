#include "MeshRender.h"

#include "Mesh.h"
#include "Material.h"

#include "GameObject.h"
#include "Transform.h"

#include "Device.h"
#include "ResMgr.h"
#include "Collider.h"
#include "Animator_2D.h"
#include "Camera.h"
#include "CamFilter.h"

#include "Shader.h"

CMeshRender::CMeshRender()
	: m_eRT(RASTERIZE_TYPE::RT_BACK)
	, m_pMesh(NULL)
	, m_bShadow(false)
	, m_bPostEffect(false)
	, m_pTrailMtrl(NULL)
{
	m_vecMtrl.resize(1);
}

CMeshRender::~CMeshRender()
{

}

void CMeshRender::Render()
{
	if (m_pMesh == NULL || m_vecMtrl.size() == 0)
		return;

	// Transform ���� update
	Transform()->UpdateData();
	
	// �����Ͷ����� ������Ʈ
	CDevice::GetInst()->SetRasterizer(m_eRT);

	//�ִϸ��̼� ó��
	if (Animator_2D())
	{
		Animator_2D()->UpdateData();
	}
	else
	{
		CAnimator_2D::Disable();
	}

	// ���׸��� �޽� ���� ó��
	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		// ���̴��� ������ ����
		if (NULL == m_vecMtrl[i] || nullptr == m_vecMtrl[i]->GetShader())
			return;

		m_vecMtrl[i]->UpdateData();
		m_pMesh->SetLayout(m_vecMtrl[i]->GetShader()->GetVSBlob());
		m_pMesh->UpdateData();

		m_pMesh->Render(i);
	}

#ifdef _DEBUG
	// ���࿡ Collider �� �ִٸ�
	//if (NULL != Collider())
	//	Collider()->Render();
#endif
}

void CMeshRender::Render(UINT _iMtrlIdx)
{
	if (m_pMesh == NULL || m_vecMtrl.empty())
		return;

	CDevice::GetInst()->SetRasterizer(m_eRT);

	if (m_vecMtrl[_iMtrlIdx] == NULL || m_vecMtrl[_iMtrlIdx]->GetShader() == NULL)
		return;

	m_pMesh->SetLayout(m_vecMtrl[_iMtrlIdx]->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();
	m_vecMtrl[_iMtrlIdx]->UpdateData();
	m_pMesh->Render(_iMtrlIdx);
}

void CMeshRender::SetMesh(CMesh * _pMesh)
{
	vector<CResPtr<CMaterial>>	vecMtrl = m_vecMtrl;
	m_vecMtrl.clear();

	m_pMesh = _pMesh;
	UINT iSize = m_pMesh->GetSubmeshCount();
	for (UINT i = 0; i < iSize; ++i)
	{
		if (i < vecMtrl.size())
		{
			SetMaterial(vecMtrl[i], i);
		}
		else
		{
			SetMaterial(NULL, i);
		}
	}
}

void CMeshRender::SetMaterial(CMaterial * _pMaterial, UINT _iIdx)
{
	if (m_vecMtrl.size() == _iIdx)
		m_vecMtrl.push_back(_pMaterial);
	else
		m_vecMtrl[_iIdx] = _pMaterial;
}

CMaterial * CMeshRender::GetMaterial(UINT _iIdx)
{
	if (_iIdx >= m_vecMtrl.size() || _iIdx < 0)
		return nullptr;

	if (NULL == m_vecMtrl[_iIdx])
		return NULL;

	m_vecMtrl[_iIdx] = m_vecMtrl[_iIdx]->Clone();

	CResMgr::GetInst()->AddCloneRes(m_vecMtrl[_iIdx]);

	return  m_vecMtrl[_iIdx];
}

CMaterial * CMeshRender::GetUIMaterial(UINT _iIdx)
{
	if (_iIdx >= m_vecMtrl.size() && _iIdx < 0)
		return nullptr;

	if (NULL == m_vecMtrl[_iIdx])
		return NULL;

	m_vecMtrl[_iIdx] = m_vecMtrl[_iIdx]->Clone();

	CResMgr::GetInst()->AddCloneRes(m_vecMtrl[_iIdx]);

	return  m_vecMtrl[_iIdx];
}

ULONG64 CMeshRender::GetInstID(UINT _iIdx)
{
	if (_iIdx >= m_vecMtrl.size() && _iIdx < 0)
		return 0;

	if (m_pMesh == NULL || m_vecMtrl[_iIdx] == NULL)
		return 0;

	uInstID id;
	id.sMesh = m_pMesh->GetResID();
	id.sMtrlNum = _iIdx;
	id.iMtrl = m_vecMtrl[_iIdx]->GetResID();

	return id.llID;
}

void CMeshRender::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::MESHRENDER;
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	// Mesh ����
	// Mesh �� Ű��
	// Mesh �� ����� ����
	SaveResourceKey(m_pMesh.GetTarget(), _pFile);

	// Material ���� ����
	UINT iMtrlCount = m_vecMtrl.size();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _pFile);

	// Material ���� ����
	for (UINT i = 0; i < m_vecMtrl.size(); ++i)
	{
		SaveResourceKey(m_vecMtrl[i].GetTarget(), _pFile);
	}

	// RS Type
	fwrite(&m_eRT, sizeof(UINT), 1, _pFile);
	fwrite(&m_bShadow, sizeof(bool), 1, _pFile);
	fwrite(&m_bPostEffect, sizeof(bool), 1, _pFile);
}

void CMeshRender::Load(FILE * _pFile)
{
	wstring strKey;
	
	strKey = LoadResourceKey(_pFile);
	m_pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(strKey);
			
	// ���׸��� ���� �б�
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, _pFile);

	// ������ŭ �ݺ���
	CMaterial* pMtrl = nullptr;
	m_vecMtrl.reserve(iMtrlCount);
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		strKey = LoadResourceKey(_pFile);
		pMtrl = CResMgr::GetInst()->FindMaterial(strKey);
		SetMaterial(pMtrl, i); 
	}

	fread(&m_eRT, sizeof(UINT), 1, _pFile);
	fread(&m_bShadow, sizeof(bool), 1, _pFile);
	fread(&m_bPostEffect, sizeof(bool), 1, _pFile);
}
