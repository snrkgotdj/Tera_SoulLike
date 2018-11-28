#include "ResMgr.h"

#include "ShaderMgr.h"

#include "GameObject.h"
#include "Prefab.h"
#include "Mesh.h"
#include "Material.h"
#include "Light.h"

#include "RenderMgr.h"
#include "CamFilter.h"

#include "FBXLoader.h"
#include "HDRFilter.h"
#include "UnderWaterFilter.h"
#include "MeshData.h"

CResMgr* CResMgr::pInst = nullptr;

CResMgr::CResMgr()
	: m_bChange(false)
{
}

CResMgr::~CResMgr()
{
	Safe_Delete_Map_Res(m_mapMeshData);
	Safe_Delete_Map_Res(m_mapPrefab);
	Safe_Delete_Map_Res(m_mapMesh);
	Safe_Delete_Map_Res(m_mapTex);
	Safe_Delete_Map_Res(m_mapMat);
	//Safe_Delete_Map_Res(m_mapSound);

	Safe_Delete_Vec_Res(m_vecCloneRes);
	Safe_Delete_Vec_Res(m_vecUICloneRes);
}

void CResMgr::init()
{
	CreateDefaultMesh();
	CreateDefaultMaterial();	
	CreateFilter();
	InitFMOD();
}

void CResMgr::ReleaseSound()
{
	Safe_Delete_Map_Res(m_mapSound);
	CSound::g_pSystem->close();
	CSound::g_pSystem->release();
#ifndef _DEBUG

#endif
}

void CResMgr::InitFMOD()
{
	FMOD::System_Create(&CSound::g_pSystem);

	if (NULL == CSound::g_pSystem)
		assert(NULL);

	CSound::g_pSystem->init(32, FMOD_INIT_NORMAL, NULL);
	//CSound::g_pSystem->set3DSettings(1.0f, 1.f, 1.0f);
}

int CResMgr::AddPrefab(const wstring & _strKey, const wstring& _strLayerTag, CGameObject * _pPrefabObj)
{	
	CPrefab* pPrefab = FindPrefab(_strKey);

	if (NULL != pPrefab)
		return RET_FAILED;

	pPrefab = new CPrefab(_pPrefabObj);

	pPrefab->SetKey(_strKey);
	pPrefab->SetLayerName(_strLayerTag);
	m_mapPrefab.insert(make_pair(_strKey, pPrefab));
	m_bChange = true;

	return RET_SUCCESS;
}

int CResMgr::AddMtrl(const wstring & _strKey, CMaterial * _pMtrl)
{
	CMaterial* pMtrl = FindMaterial(_strKey);

	if (NULL != pMtrl)
		return RET_FAILED;
	
	_pMtrl->SetKey(_strKey);
	m_mapMat.insert(make_pair(_strKey, _pMtrl));
	m_bChange = true;

	return RET_SUCCESS;
}

CMaterial * CResMgr::FindMaterial(const wstring & _strKey)
{
	map<wstring, CResPtr<CMaterial>>::iterator iter = m_mapMat.find(_strKey);

	if (iter == m_mapMat.end())
		return NULL;

	return iter->second;
}

CMaterial * CResMgr::GetCloneMaterial(const wstring & _strKey)
{
	CMaterial* pMtrl = FindMaterial(_strKey);
	if (pMtrl == nullptr)
		return pMtrl;

	pMtrl = pMtrl->Clone();
	m_vecCloneRes.push_back(pMtrl);
	return pMtrl;
}

int CResMgr::AddMesh(const wstring& _strKey, CMesh * _pMesh)
{
	CMesh* pMesh = FindMesh(_strKey);

	if (NULL != pMesh)
	{
		return RET_FAILED;
	}
		
	_pMesh->SetKey(_strKey);
	m_mapMesh.insert(make_pair(_strKey, _pMesh));

	m_bChange = true;

	return RET_SUCCESS;
}

CMesh * CResMgr::FindMesh(const wstring & _strKey)
{
	map<wstring, CResPtr<CMesh>>::iterator iter = m_mapMesh.find(_strKey);

	if (iter == m_mapMesh.end())
		return NULL;

	return iter->second;
}

CTexture * CResMgr::FindTexture(const wstring & _strKey)
{
	map<wstring, CResPtr<CTexture>>::iterator iter = m_mapTex.find(_strKey);

	if (iter == m_mapTex.end())
		return NULL;

	return iter->second;
}

CPrefab * CResMgr::FindPrefab(const wstring & _strKey)
{
	map<wstring, CResPtr<CPrefab>>::iterator iter = m_mapPrefab.find(_strKey);

	if (iter == m_mapPrefab.end())
		return NULL;

	return iter->second;
}

CSound * CResMgr::FindSound(const wstring & _strKey)
{
	map<wstring, CResPtr<CSound>>::iterator iter = m_mapSound.find(_strKey);

	if (iter == m_mapSound.end())
		return NULL;

	return iter->second;
}

CMeshData * CResMgr::FindMeshData(const wstring & _strKey)
{
	map<wstring, CResPtr<CMeshData>>::iterator iter = m_mapMeshData.find(_strKey);

	if (iter == m_mapMeshData.end())
		return NULL;

	return iter->second;
}

void CResMgr::CreateDefaultMesh()
{
	CreateSpotMesh();
	CreateTriangleMesh();
	CreateRectMesh();
	CreateBox();
	CreateSphere();
	CreateLightRangeMesh();
}

void CResMgr::CreateSpotMesh()
{
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 1.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);
	vecVtx.push_back(v);

	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(3);


	CResPtr<CMesh> pMesh = CMesh::Create(vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0], D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//pMesh->m_bTest = true;

	AddMesh(L"SpotMesh", pMesh);
}

void CResMgr::CreateTriangleMesh()
{
	//=====================
	// Create Tri Mesh
	//=====================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	v.vPos = Vec3(-0.5f, 0.f, 0.f);
	v.vUV = Vec2(0.f, 1.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.f, 0.5f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.f, 0.f);
	v.vUV = Vec2(1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(3);

	CResPtr<CMesh> pMesh = CMesh::Create(vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0], D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	AddMesh(L"TriMesh", pMesh);
}

void CResMgr::CreateRectMesh()
{
	//=====================
	// Create Rect Mesh
	//=====================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	v.vPos = Vec3(-0.5f, -0.5f, 0.f);
	v.vUV = Vec2(0.f, 1.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, 0.5f, 0.f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(0.f, 1.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.f);
	v.vUV = Vec2(1.f, 0.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.f);
	v.vUV = Vec2(1.f, 1.f);
	v.vColor = Vec4(0.f, 0.f, 1.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(3);
	vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(3);

	CResPtr<CMesh> pMesh = CMesh::Create(vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]);

	AddMesh(L"RectMesh", pMesh);

	//==========================
	// Create Collider Rect Mesh
	//==========================
	vecIdx.clear();

	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(3); vecIdx.push_back(0);

	pMesh = CMesh::Create(vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]
		, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	AddMesh(L"ColliderRectMesh", pMesh);
}

void CResMgr::CreateBox()
{
	//=================
	// Create Box Mesh
	//=================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	v.vPos = Vec3(-0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);

	// ¾Æ·§ ¸é
	v.vPos = Vec3(-0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// ¿À¸¥ÂÊ
	v.vPos = Vec3(0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	// ¿ÞÂÊ ¸é
	v.vPos = Vec3(-0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(-1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(-1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(-1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(-1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(-1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(-1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(-1.f, 0.f, 0.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(-1.f, 0.f, 0.f);
	vecVtx.push_back(v);

	// ¾Õ ¸é
	v.vPos = Vec3(-0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, 1.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, 1.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, 1.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, 1.f);
	v.vTangent = Vec3(0.f, 0.f, -1.f);
	v.vBinormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	// µÞ ¸é
	v.vPos = Vec3(-0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 0.f, -1.f);
	v.vTangent = Vec3(0.f, 0.f, 1.f);
	v.vBinormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);


	// ÀÎµ¦½º
	// À­ ¸é ÀÎµ¦½º
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);
	vecIdx.push_back(0); vecIdx.push_back(2); vecIdx.push_back(3);

	// ¾Æ·§ ¸é ÀÎµ¦½º
	vecIdx.push_back(6); vecIdx.push_back(5); vecIdx.push_back(4);
	vecIdx.push_back(6); vecIdx.push_back(4); vecIdx.push_back(7);

	// ¿À¸¥ÂÊ ¸é ÀÎµ¦½º
	vecIdx.push_back(8); vecIdx.push_back(9); vecIdx.push_back(10);
	vecIdx.push_back(8); vecIdx.push_back(10); vecIdx.push_back(11);

	// ¿ÞÂÊ ¸é
	vecIdx.push_back(13); vecIdx.push_back(12); vecIdx.push_back(15);
	vecIdx.push_back(13); vecIdx.push_back(15); vecIdx.push_back(14);

	// ¾Õ ¸é
	vecIdx.push_back(17); vecIdx.push_back(16); vecIdx.push_back(19);
	vecIdx.push_back(17); vecIdx.push_back(19); vecIdx.push_back(18);

	// µÞ ¸é
	vecIdx.push_back(20); vecIdx.push_back(21); vecIdx.push_back(22);
	vecIdx.push_back(20); vecIdx.push_back(22); vecIdx.push_back(23);

	CResPtr<CMesh> pMesh = CMesh::Create(
		vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, (UINT)vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]);

	AddMesh(L"BoxMesh", pMesh);

	vecVtx.clear(); vecIdx.clear();

	// À­ ¸é
	v.vPos = Vec3(-0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, 0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	vecVtx.push_back(v);

	// ¾Æ·§ ¸é
	v.vPos = Vec3(-0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(-0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, -0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);

	v.vPos = Vec3(0.5f, -0.5f, 0.5f);
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	v.vNormal = Vec3(0.f, -1.f, 0.f);
	vecVtx.push_back(v);


	// ÀÎµ¦½º

	// À­ ¸é
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2); vecIdx.push_back(3); vecIdx.push_back(0);

	// ¿ÞÂÊ ¸é
	vecIdx.push_back(4); vecIdx.push_back(5); vecIdx.push_back(1);

	// µÞ ¸é
	vecIdx.push_back(1); vecIdx.push_back(5); vecIdx.push_back(6); vecIdx.push_back(2);

	// ¿À¸¥ÂÊ ¸é
	vecIdx.push_back(2); vecIdx.push_back(6); vecIdx.push_back(7); vecIdx.push_back(3);

	// ¾Õ ¸é
	vecIdx.push_back(3); vecIdx.push_back(7); vecIdx.push_back(4); vecIdx.push_back(0);

	pMesh = CMesh::Create(
		vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, (UINT)vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]
		, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	AddMesh(L"ColliderBoxMesh", pMesh);
}

void CResMgr::CreateSphere()
{
	//====================
	// Create Sphere Mesh
	//====================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	float fRadius = 1.f;

	// Top
	v.vPos = Vec3(0.f, fRadius, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, 1.f);
	vecVtx.push_back(v);
	
	// Body
	UINT iStackCount = 40; // °¡·Î ºÐÇÒ °³¼ö
	UINT iSliceCount = 40; // ¼¼·Î ºÐÇÒ °³¼ö

	float fStackAngle = XM_PI / iStackCount;
	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;
	float fUVYStep = 1.f / (float)iStackCount;

	for (UINT i = 1; i < iStackCount; ++i)
	{
		float phi = i * fStackAngle;

		for (UINT j = 0; j <= iSliceCount; ++j)
		{
			float theta = j * fSliceAngle;

			v.vPos = Vec3(fRadius * sinf(i * fStackAngle) * cosf(j * fSliceAngle)
				, fRadius * cosf(i * fStackAngle)
				, fRadius * sinf(i * fStackAngle) * sinf(j * fSliceAngle));
			v.vUV = Vec2(fUVXStep * j, fUVYStep * i);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = v.vPos;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(phi) * sinf(theta);
			v.vTangent.y = 0.f;
			v.vTangent.z = fRadius * sinf(phi) * cosf(theta);
			v.vTangent = XMVector3Normalize(v.vTangent.Convert());

			v.vBinormal = XMVector3Cross(v.vTangent.Convert(), v.vNormal.Convert());
			v.vBinormal = XMVector3Normalize(v.vBinormal.Convert());

			vecVtx.push_back(v);
		}
	}
	
	// Bottom
	v.vPos = Vec3(0.f, -fRadius, 0.f);
	v.vUV = Vec2(0.5f, 1.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	v.vNormal = v.vPos;
	v.vNormal.Normalize();
	v.vTangent = Vec3(-1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	vecVtx.push_back(v);

	// ÀÎµ¦½º
	// ºÏ±ØÁ¡
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	// ¸öÅë
	for (UINT i = 0; i < iStackCount - 2; ++i)
	{
		for (UINT j = 0; j < iSliceCount; ++j)
		{
			// + 
			// | \
			// +--+
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j)+1);

			// +--+
			//  \ |
			//    +
			vecIdx.push_back((iSliceCount + 1) * (i)+(j)+1);
			vecIdx.push_back((iSliceCount + 1) * (i)+(j + 1) + 1);
			vecIdx.push_back((iSliceCount + 1) * (i + 1) + (j + 1) + 1);		
		}
	}

	// ³²±ØÁ¡
	UINT iBottomIdx = (UINT)vecVtx.size() - 1;
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(iBottomIdx);
		vecIdx.push_back(iBottomIdx - (i + 2));
		vecIdx.push_back(iBottomIdx - (i + 1));
	}

	CResPtr<CMesh> pMesh = CMesh::Create(
		vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, (UINT)vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]);

	AddMesh(L"SphereMesh", pMesh);
}

void CResMgr::CreateLightRangeMesh()
{
	//=====================
	// Sphere Mesh
	//=====================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	float fRadius = 1.f;

	// Center
	v.vPos = Vec3(0.f, 0.f, 0.f);
	v.vUV = Vec2(0.5f, 0.f);
	v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
	vecVtx.push_back(v);

	// Body
	UINT iSliceCount = 400; // ¼¼·Î ºÐÇÒ °³¼ö

	float fSliceAngle = XM_2PI / iSliceCount;

	float fUVXStep = 1.f / (float)iSliceCount;

	for (UINT i = 0; i < iSliceCount + 1; ++i)
	{
		v.vPos = Vec3(fRadius*cosf(i * fSliceAngle), fRadius*sinf(i * fSliceAngle), 0.f);
		v.vUV = Vec2(fUVXStep * i, fUVXStep * i);
		v.vColor = Vec4(1.f, 0.5f, 0.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();

		vecVtx.push_back(v);
	}

	// ÀÎµ¦½º
	// ºÏ±ØÁ¡
	for (UINT i = 0; i < iSliceCount; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	for (UINT i = iSliceCount + 1; i < (iSliceCount + 1) * 2; ++i)
	{
		v.vPos = Vec3(fRadius*cosf(i * fSliceAngle), 0.f, fRadius*sinf(i * fSliceAngle));
		v.vUV = Vec2(fUVXStep * i, fUVXStep * i);
		v.vColor = Vec4(1.f, 0.5f, 0.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();

		vecVtx.push_back(v);
	}

	// ÀÎµ¦½º
	// ºÏ±ØÁ¡
	for (UINT i = iSliceCount; i < iSliceCount * 2; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	for (UINT i = (iSliceCount + 1) * 2; i < (iSliceCount + 1) * 3; ++i)
	{
		v.vPos = Vec3(0.f, fRadius*cosf(i * fSliceAngle), fRadius*sinf(i * fSliceAngle));
		v.vUV = Vec2(fUVXStep * i, fUVXStep * i);
		v.vColor = Vec4(1.f, 0.5f, 0.f, 1.f);
		v.vNormal = v.vPos;
		v.vNormal.Normalize();

		vecVtx.push_back(v);
	}

	// ÀÎµ¦½º
	// ºÏ±ØÁ¡
	for (UINT i = iSliceCount * 2; i < iSliceCount * 3; ++i)
	{
		vecIdx.push_back(0);
		vecIdx.push_back(i + 2);
		vecIdx.push_back(i + 1);
	}

	CResPtr<CMesh> pMesh = CMesh::Create(
		vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT, &vecVtx[0]
		, (UINT)vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0]);

	AddMesh(L"LightRange_SphereMesh", pMesh);
}

void CResMgr::CreateDefaultMaterial()
{
	CMaterial* pMtrl = new CMaterial;
	CTexture* pTargetTex = nullptr;

	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Color"));
	AddMtrl(L"Mtrl_Default", pMtrl);

	//===================
	// Collider Material
	//===================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Collider"));	
	AddMtrl(L"Mtrl_Collider", pMtrl);

	//================
	// Std2D Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Std2D"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Std2D", pMtrl);

	//================
	// Std2D OneBlend Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Std2D_OneBlend"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Std2D_One", pMtrl);

	//======================
	// Black Effect Material
	//======================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_BlackEffect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_BlackEffect", pMtrl);
	CTexture* pTexture = CResMgr::GetInst()->FindTexture(L"RenderTarget_1");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTexture);

	//================
	// Std3D Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Std3D"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Std3D", pMtrl);
	pTargetTex = CResMgr::GetInst()->FindTexture(TARGET_DIR_SHADOWMAP);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTargetTex);

	//======================
	// StdDeferred Material
	//======================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_StdDeferred"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_StdDeferred", pMtrl);

	//================
	// Tex Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Tex"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Tex", pMtrl);

	//================
	// SkyBox Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Sky"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Sky", pMtrl);

	//================
	// SkyBox Cube Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_SkyCube"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_SkyCube", pMtrl);

	//=================
	// Terrain Material
	//=================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Terrain"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Terrain", pMtrl);

	//=================
	// Tessellation Material
	//=================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Tessellation"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Tessellation", pMtrl);

	//==================
	// Gaussian Material
	//==================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Gaussian"));
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Gaussian", pMtrl);

	//==================
	// DepthOfField Material
	//==================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_DepthOfField"));
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_DEPTH);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_DepthOfField", pMtrl);

	//==================
	// Fog Material
	//==================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Fog"));
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_DEPTH);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Fog", pMtrl);

	//==================
	// Fog Material
	//==================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_BloodScreen"));
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	pTargetTex = (CTexture*)Load<CTexture>(L"BloodScreen",L"Texture\\ScreenBlood.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_BloodScreen", pMtrl);

	//====================
	// Distortion Material
	//=====================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Distortion"));
	pTargetTex = (CTexture*)Load<CTexture>(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Distortion", pMtrl);

	// Detort Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Detort"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Detort", pMtrl);

	// OldFilm Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_OldFilm"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_OldFilm", pMtrl);

	// UnderWater Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_UnderWater"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_UnderWater", pMtrl);

	// Ripple Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Ripple"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Ripple", pMtrl);

	// MotionBlur Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_MotionBlur"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	pTargetTex = FindTexture(TARGET_VELOCITY);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_MotionBlur", pMtrl);

	// Fade Material
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Fade"));
	pTargetTex = FindTexture(TARGET_POST);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Fade", pMtrl);

	//================
	// HDR Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_HDR"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_HDR", pMtrl);

	//================
	// Volume Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Volume"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Volume", pMtrl);

	//============================
	// Light Range Sphere Material
	//============================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_RangeSphere"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_RangeSphere", pMtrl);

	//================
	// Light Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_LightDraw"));

	pTargetTex = (CTexture*)Load<CTexture>(L"RenderTarget_3");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(L"RenderTarget_4");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(L"RenderTarget_5");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(TARGET_DIR_SHADOWMAP);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_3, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(TARGET_SPECULAR);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_4, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(TARGET_EMISSIVE);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_5, &pTargetTex);

	CResMgr::GetInst()->AddMtrl(L"Mtrl_Light", pMtrl);

	//=====================
	// Light Merge Material
	//=====================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_LightMerge"));

	pTargetTex = (CTexture*)Load<CTexture>(L"RenderTarget_2");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(L"RenderTarget_6");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTargetTex);

	pTargetTex = (CTexture*)Load<CTexture>(TARGET_LIGHT_SPECULAR);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTargetTex);

	CResMgr::GetInst()->AddMtrl(L"Mtrl_LightMerge", pMtrl);

	//============================
	// Button Material
	//============================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Button"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Button", pMtrl);

	//======================
	// NavMesh Material
	//======================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_NavMesh"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_NavMesh", pMtrl);

	// Åø ¸ÞÅ×¸®¾ó

	//================
	// Grid Material
	//================	
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Grid"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Grid", pMtrl);

	//=========================
	// Forward Target Material
	//=========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_ToolUI"));
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"RenderTarget_1");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
	CResMgr::GetInst()->AddMtrl(L"Mtrl_ToolUI", pMtrl);

	//========================
	// Dir Shadow Map Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_DirShadowMap"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_DirShadowMap", pMtrl);

	//========================
	// Dir Shadow Map Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_TestGeo"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_TestGeo", pMtrl);

	//========================
	// StreamOut Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_StreamOut"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_StreamOut", pMtrl);

	//========================
	// DrawParticle Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_DrawParticle"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_DrawParticle", pMtrl);

	//========================
	// Water Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Water"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Water", pMtrl);

	//========================
	// Trail Material
	//========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Trail"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Trail", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTexture);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Wave", L"Texture\\Effect\\Wave.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTexture);

	//===============================
	// Distortion Effect Material
	//=============================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Distortion_Effect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Distortion_Effect", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTexture);

	//===============================
	// Electric Effect Material
	//=============================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Electric_Effect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Electric_Effect", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTexture);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Wave", L"Texture\\Effect\\Wave.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTexture);

	//===============================
	// Wave Effect Material
	//=============================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Wave_Effect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Wave_Effect", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTexture);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Wave", L"Texture\\Effect\\Wave.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTexture);

	//================
	// BurnOut Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_BurnOut"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_BurnOut", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_DIR_SHADOWMAP);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTexture);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Noise_01", L"Texture\\Noise\\Noise_01.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_3, &pTexture);

	//================
	// AfterImage Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_AfterImage"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_AfterImage", pMtrl);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Noise_01", L"Texture\\Noise\\Noise_01.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTexture);

	//================
	// Tile Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Tile_Effect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Tile_Effect", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_2, &pTexture);

	//================
	// SoftParticle Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_SoftParticle"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_SoftParticle", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_DEPTH);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTexture);

	//================
	// HP Bar Material
	//================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_HP"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_HP", pMtrl);
	pTexture = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Wave", L"Texture\\Effect\\Wave.png");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTexture);

	//===========================
	// Mtrl_Potal_Effect Material
	//===========================
	pMtrl = new CMaterial;
	pMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Potal_Effect"));
	CResMgr::GetInst()->AddMtrl(L"Mtrl_Potal_Effect", pMtrl);
	pTexture = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTexture);
}

void CResMgr::CreateFilter()
{
	// Gaussian Blur Filter
	CCamFilter* pFilter = NULL;

	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Gaussian"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_Gaussian", pFilter);

	// DOF Filter
	pFilter = NULL;

	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_DepthOfField"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_DepthOfField", pFilter);

	// FOG Filter
	pFilter = NULL;

	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Fog"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_Fog", pFilter);

	// FOG Filter
	pFilter = NULL;

	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_BloodScreen"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_BloodScreen", pFilter);

	// Distortion Filter
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Distortion"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_Distortion", pFilter);

	// Detort Filter
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Detort"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	CRenderMgr::GetInst()->AddFilter(L"Filter_Detort", pFilter);

	// OldFilm Filter
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_OldFilm"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	CRenderMgr::GetInst()->AddFilter(L"Filter_OldFilm", pFilter);

	// UnderWater Filter
	pFilter = new CUnderWaterFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_UnderWater"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	CRenderMgr::GetInst()->AddFilter(L"Filter_UnderWater", pFilter);

	// Ripple Filter
	pFilter = new CUnderWaterFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Ripple"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	CRenderMgr::GetInst()->AddFilter(L"Filter_Ripple", pFilter);

	// HDR Filter
	pFilter = new CHDRFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_HDR"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	CRenderMgr::GetInst()->AddFilter(L"Filter_HDR", pFilter);

	// Light Filter
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Light"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	pFilter->FinalUpdate();

	CRenderMgr::GetInst()->AddFilter(L"Filter_Light", pFilter);
	CLight::g_pLightFilter = pFilter;

	// Volume Mesh Filter
	// Point Light
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"SphereMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Volume"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"Filter_PointLight", pFilter);

	// Spot Light
	/*pFilter = new CLightFilter;
	pFilter->SetMesh(FindMesh(L"ConMesh"));
	pFilter->SetMaterial(FindMaterial(L"DirLightMtrl"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));

	CRenderMgr::GetInst()->AddFilter(L"SpotLightFilter", pFilter);*/

	// Light Merge Filter	
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_LightMerge"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	pFilter->FinalUpdate();
	CRenderMgr::GetInst()->AddFilter(L"Filter_LightMerge", pFilter);
	CRenderMgr::GetInst()->SetLightMergeFilter(pFilter);

	// MotionBlur Filter	
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_MotionBlur"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	pFilter->FinalUpdate();
	CRenderMgr::GetInst()->AddFilter(L"Filter_MotionBlur", pFilter);

	// Fade Filter	
	pFilter = new CCamFilter;
	pFilter->SetMesh(FindMesh(L"RectMesh"));
	pFilter->SetMaterial(FindMaterial(L"Mtrl_Fade"));
	pFilter->SetScale(Vec3(2.f, 2.f, 2.f));
	pFilter->FinalUpdate();
	CRenderMgr::GetInst()->AddFilter(L"Filter_Fade", pFilter);
}

CTexture * CResMgr::CreateTexture(const wstring & _strKey
	, UINT _iWidth, UINT _iHeight
	, UINT _iBindFlag, DXGI_FORMAT _eFormat
	, D3D11_USAGE _eUsage)
{
	CTexture* pTex = FindTexture(_strKey);

	if (NULL != pTex)
		return pTex;

	pTex = CTexture::Create(_iWidth, _iHeight, _iBindFlag, _eFormat, _eUsage);
	m_mapTex.insert(make_pair(_strKey, pTex));
	pTex->SetKey(_strKey);

	return pTex;
}

CTexture * CResMgr::CreateTexture(const wstring & _strKey, ID3D11Texture2D * _pTex2D, UINT _iBindFlag, DXGI_FORMAT _eFormat)
{
	CTexture* pTex = FindTexture(_strKey);

	if (NULL != pTex)
		return pTex;

	pTex = CTexture::Create(_pTex2D, _iBindFlag, _eFormat);
	m_mapTex.insert(make_pair(_strKey, pTex));
	pTex->SetKey(_strKey);

	return pTex;
}

CTexture * CResMgr::CreateMultiTexture(const wstring & _strKey, vector<CTexture*>& _vecTex, D3D11_USAGE _eUsage)
{
	map<wstring, CResPtr<CTexture>>::iterator iter = m_mapTex.find(_strKey);
	if (iter != m_mapTex.end())
		return iter->second;

	CTexture* pTex = CTexture::Create(_vecTex, _eUsage);
	pTex->SetKey(_strKey);
	m_mapTex.insert(make_pair(_strKey, pTex));
	return pTex;
}

CMesh * CResMgr::CreatePlaneMesh(const wstring & _strKey, UINT _iXFace, UINT _iZFace, D3D11_PRIMITIVE_TOPOLOGY _eTopology)
{
	//=====================
	// Create Plane Mesh
	//=====================
	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	// Face Counts
	UINT iXCount = _iXFace;
	UINT iZCount = _iZFace;

	for (UINT i = 0; i < iZCount + 1; ++i)
	{
		for (UINT j = 0; j < iXCount + 1; ++j)
		{
			float iX = (float)j;
			float iY = (float)i;
			v.vPos = Vec3(iX, 0.f, iY);
			v.vUV = Vec2(iX, iZCount - iY);
			v.vColor = Vec4(1.f, 1.f, 1.f, 1.f);
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, 1.f);

			vecVtx.push_back(v);
		}
	}

	for (UINT i = 0; i < iZCount; ++i)
	{
		for (UINT j = 0; j < iXCount; ++j)
		{
			vecIdx.push_back((iXCount + 1) * (i + 1) + j);
			vecIdx.push_back((iXCount + 1) * i + j + 1);
			vecIdx.push_back((iXCount + 1) * i + j);

			vecIdx.push_back((iXCount + 1) * i + j + 1);
			vecIdx.push_back((iXCount + 1) * (i + 1) + j);
			vecIdx.push_back((iXCount + 1) * (i + 1) + j + 1);
		}
	}


	CResPtr<CMesh> pMesh = CMesh::Create(vecVtx.size(), (UINT)sizeof(VTX), D3D11_USAGE_DEFAULT
		, &vecVtx[0], (UINT)vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DEFAULT, INDEX16::format(), &vecIdx[0], _eTopology);

	AddMesh(_strKey, pMesh);

	return pMesh;
}

void CResMgr::DeleteCloneMtrl(CMaterial* _pMtrl)
{
	if (m_vecCloneRes.empty())
		return;

	vector<CResPtr<CRes>>::iterator iter = m_vecCloneRes.begin();

	for (; iter != m_vecCloneRes.end(); ++iter)
	{
		if (_pMtrl == (*iter))
		{
			delete *iter;
			m_vecCloneRes.erase(iter);
			return;
		}
	}
}

CSound * CResMgr::LoadSound(const wstring & _strKey, const wstring & _strFilePath, bool _bIs3D)
{
	CSound* pSound = FindSound(_strKey);

	if (NULL != pSound)
		return pSound;

	wstring strFullPath = CPathMgr::GetResPath();
	strFullPath += _strFilePath;

	pSound = CSound::Create(strFullPath, _bIs3D);

	if (NULL == pSound)
		return NULL;

	pSound->SetKey(_strKey);
	pSound->SetPath(_strFilePath);
	m_mapSound.insert(make_pair(_strKey, pSound));

	m_bChange = true;

	return pSound;
}

void CResMgr::DeletePrefab(const wstring& _strKey)
{
	map<wstring, CResPtr<CPrefab>>::iterator iter = m_mapPrefab.find(_strKey);
	if (iter == m_mapPrefab.end())
		return;

	delete iter->second;
	m_mapPrefab.erase(iter);
}

CMeshData* CResMgr::LoadFBX(const wstring & _strPath)
{
	CMeshData* pMeshData = CMeshData::LoadFromFBX(_strPath);

	wstring strFileName = CPathMgr::GetFileName(_strPath.c_str());

	if (m_mapMeshData.find(strFileName) != m_mapMeshData.end())
		assert(NULL);

	pMeshData->SetKey(strFileName);
	m_mapMeshData.insert(make_pair(strFileName, pMeshData));

	return pMeshData;
}

CMeshData * CResMgr::LoadMeshData(const wstring & _strPath)
{
	CMeshData* pMeshData = CMeshData::LoadFromMeshData(_strPath);

	wstring strFileName = CPathMgr::GetFileName(_strPath.c_str());

	if (m_mapMeshData.find(strFileName) != m_mapMeshData.end())
		assert(NULL);

	pMeshData->SetKey(strFileName);
	m_mapMeshData.insert(make_pair(strFileName, pMeshData));

	return pMeshData;
}

void CResMgr::SaveResource(FILE * _pFile)
{
	// Mesh
	UINT iCount = (UINT)m_mapMesh.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (auto& pair : m_mapMesh)
	{
		// ref count ÀúÀå
		int iRefCount = pair.second->GetRefCount();
		fwrite(&iRefCount, sizeof(int), 1, _pFile);

		if(1 < iRefCount)
			pair.second->Save(_pFile);
	}

	// Texture
	iCount = (UINT)m_mapTex.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (auto& pair : m_mapTex)
	{
		// ref count ÀúÀå
		int iRefCount = pair.second->GetRefCount();
		fwrite(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
			pair.second->Save(_pFile);
	}

	// Material
	iCount = (UINT)m_mapMat.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (auto& pair : m_mapMat)
	{
		// ref count ÀúÀå
		int iRefCount = pair.second->GetRefCount();
		fwrite(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
			pair.second->Save(_pFile);
	}

	// Sound
	iCount = (UINT)m_mapSound.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (auto& pair : m_mapSound)
	{
		// ref count ÀúÀå
		int iRefCount = pair.second->GetRefCount();
		fwrite(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
			pair.second->Save(_pFile);
	}
}

void CResMgr::LoadResource(FILE * _pFile)
{
	wstring strKey;
	wstring strPath;

	// Mesh
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		int iRefCount = 0;
		fread(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
		{
			CMesh::Load(_pFile);
		}
	}

	// Texture
	iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		int iRefCount = 0;
		fread(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
		{
			strKey = LoadWString(_pFile);
			strPath = LoadWString(_pFile);
			Load<CTexture>(strKey, strPath);
		}
	}

	// Material
	iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		int iRefCount = 0;
		fread(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
		{
			strKey = LoadWString(_pFile);
			strPath = LoadWString(_pFile);

			CMaterial* pMtrl = FindMaterial(strKey);
			if (NULL == pMtrl)
			{
				pMtrl = new CMaterial;
				AddMtrl(strKey, pMtrl);
			}
						
			pMtrl->LoadShaderParam(_pFile);
		}
	}

	// Sound
	iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		int iRefCount = 0;
		fread(&iRefCount, sizeof(int), 1, _pFile);

		if (1 < iRefCount)
		{
			strKey = LoadWString(_pFile);
			strPath = LoadWString(_pFile);
			Load<CSound>(strKey, strPath);
		}
	}
}
