#include "MeshData.h"

#include "PathMgr.h"
#include "ResMgr.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"
#include "Animator_3D.h"

CMeshData::CMeshData()
{
}

CMeshData::~CMeshData()
{
}

CMeshData * CMeshData::LoadFromFBX(const wstring& _strPath)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strPath;

	CFBXLoader loader;
	loader.init();
	loader.LoadFbx(strFullPath);

	// 메쉬 가져오기
	CMesh* pMesh = CMesh::CreateFromContainer(loader);
	CResMgr::GetInst()->AddMesh(CPathMgr::GetFileName(_strPath.c_str()), pMesh);

	vector<CResPtr<CMaterial>> vecMtrl;
	// 메테리얼 가져오기
	for (UINT i = 0; i < loader.GetContainer(0).vecMtrl.size(); ++i)
	{
		CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(loader.GetContainer(0).vecMtrl[i].strMtrlName);
		vecMtrl.push_back(pMtrl);
	}

	CMeshData* pMeshData = new CMeshData;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}

void CMeshData::SaveMeshData()
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += L"Mesh\\MeshData\\";
	strFullPath += GetKey();
	strFullPath += L".mdat";

	FILE* pFile = NULL;
	_wfopen_s(&pFile, strFullPath.c_str(), L"wb");

	// Mesh Key 저장	
	// Mesh Data 저장
	m_pMesh->Save(pFile);

	// material 정보 저장
	UINT iMtrlCount = m_vecMtrl.size();
	fwrite(&iMtrlCount, sizeof(UINT), 1, pFile);
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		m_vecMtrl[i]->Save(pFile);
	}

	fclose(pFile);
}

CGameObject * CMeshData::Instantiate()
{
	CGameObject* pNewObj = new CGameObject;
	pNewObj->AddComponent<CTransform>(new CTransform);
	pNewObj->AddComponent<CMeshRender>(new CMeshRender);

	pNewObj->MeshRender()->SetMesh(m_pMesh);
	pNewObj->MeshRender()->SetCastShadow(true);

	UINT iSize = m_vecMtrl.size();
	pNewObj->MeshRender()->MaterialResize(iSize);
	for (UINT i = 0; i < iSize; ++i)
	{
		pNewObj->MeshRender()->SetMaterial(m_vecMtrl[i], i);
	}

	if (m_pMesh->GetBones()->empty())
		return pNewObj;

	// Animation
	CTexture* pBoneTex =
		CResMgr::GetInst()->CreateTexture(m_pMesh->GetKey()
			, m_pMesh->GetBones()->size() * 4, 1
			, D3D11_BIND_SHADER_RESOURCE
			, DXGI_FORMAT_R32G32B32A32_FLOAT
			, D3D11_USAGE_DYNAMIC);

	m_pMesh->SetBoneTex(pBoneTex);

	CAnimator_3D* pAnimator = new CAnimator_3D;
	pNewObj->AddComponent<CAnimator_3D>(pAnimator);

	pAnimator->SetBones(m_pMesh->GetBones());
	pAnimator->SetAnimClip(m_pMesh->GetAnimClip());
	pAnimator->SetBoneTex(m_pMesh->GetBoneTex());

	return pNewObj;
}


CMeshData * CMeshData::LoadFromMeshData(const wstring & _strFilePath)
{
	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strFilePath;

	FILE* pFile = NULL;
	_wfopen_s(&pFile, strFullPath.c_str(), L"rb");

	// Mesh Load
	CMesh* pMesh = CMesh::Load(pFile);

	// material 정보 읽기
	int iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(UINT), 1, pFile);
	vector<CResPtr<CMaterial>> vecMtrl;
	for (int i = 0; i < iMtrlCount; ++i)
	{
		CMaterial* pMtrl = new CMaterial;

		wstring strKey = LoadWString(pFile);
		wstring strPath = LoadWString(pFile);

		pMtrl->LoadShaderParam(pFile);

		if (RET_FAILED == CResMgr::GetInst()->AddMtrl(strKey, pMtrl))
		{
			delete pMtrl;
			pMtrl = CResMgr::GetInst()->FindMaterial(strKey);
		}

		vecMtrl.push_back(pMtrl);
	}

	fclose(pFile);

	CMeshData* pMeshData = new CMeshData;
	pMeshData->m_pMesh = pMesh;
	pMeshData->m_vecMtrl = vecMtrl;

	return pMeshData;
}