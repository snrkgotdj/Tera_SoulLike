#pragma once

#include "PathMgr.h"
#include "global.h"
#include "resptr.h"
#include "Texture.h"
#include "Sound.h"


class CMesh;
class CPrefab;
class CMeshInfo;
class CGameObject;
class CMaterial;
class CSound;
class CMeshData;

class CResMgr
{

private:
	map<wstring, CResPtr<CMesh>>		m_mapMesh;
	map<wstring, CResPtr<CTexture>>		m_mapTex;
	map<wstring, CResPtr<CPrefab>>		m_mapPrefab;
	map<wstring, CResPtr<CMaterial>>	m_mapMat;
	map<wstring, CResPtr<CSound>>		m_mapSound;
	map<wstring, CResPtr<CMeshData>>	m_mapMeshData; //Ref Count ¡÷¿«

	vector<CResPtr<CRes>>			    m_vecCloneRes;
	vector<CResPtr<CMaterial>>			m_vecUICloneRes;

	bool								m_bChange;

public:
	void init();
	void ReleaseSound();

public:
	CMaterial* FindMaterial(const wstring& _strKey);
	CMaterial* GetCloneMaterial(const wstring& _strKey);
	CMesh* FindMesh(const wstring& _strKey);
	CTexture* FindTexture(const wstring& _strKey);
	CPrefab* FindPrefab(const wstring& _strKey);
	CSound* FindSound(const wstring& _strKey);
	CMeshData* FindMeshData(const wstring& _strKey);

	int AddPrefab(const wstring& _strKey, const wstring& _strLayerTag, CGameObject* _pPrefab);
	int AddMesh(const wstring& _strKey, CMesh* _pMesh);
	int AddMtrl(const wstring& _strKey, CMaterial* _pMtrl);
	void AddCloneRes(CRes* _pRes) { m_vecCloneRes.push_back(_pRes); }
	void AddUICloneRes(CMaterial* _pRes) { m_vecUICloneRes.push_back(_pRes); }
	void ResetUI() { Safe_Delete_Vec_Res(m_vecUICloneRes); }

	map<wstring, CResPtr<CMesh>>& GetMeshMap() { return m_mapMesh; }
	map<wstring, CResPtr<CTexture>>& GetTextureMap() {return m_mapTex;	}
	map<wstring, CResPtr<CPrefab>>& GetPrefabMap() { return m_mapPrefab; }
	map<wstring, CResPtr<CMaterial>>& GetMtrlMap() { return m_mapMat; }
	map<wstring, CResPtr<CSound>>& GetSoundMap() { return m_mapSound; }
	vector<CResPtr<CRes>>& GetCloneMap() { return m_vecCloneRes; }

	void DeletePrefab(const wstring& _strKey);

private:
	void CreateDefaultMesh();
	void CreateSpotMesh();
	void CreateTriangleMesh();
	void CreateRectMesh();
	void CreateBox();
	void CreateSphere();
	void CreateLightRangeMesh();

	void CreateDefaultMaterial();
	void CreateFilter();
	void InitFMOD();

public:
	bool IsChange() { return m_bChange; }
	void SetChange(bool _bChange) { m_bChange = _bChange; }
	CTexture* CreateTexture(const wstring& _strKey
		, UINT _iWidth, UINT _iHeight
		, UINT _iBindFlag, DXGI_FORMAT _eFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM
		, D3D11_USAGE _eUsage = D3D11_USAGE::D3D11_USAGE_DEFAULT);

	CTexture* CreateTexture(const wstring& _strKey
		, ID3D11Texture2D* _pTex2D
		, UINT _iBindFlag, DXGI_FORMAT _eFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

	CTexture* CreateMultiTexture(const wstring & _strKey, vector<CTexture*>& _vecTex, D3D11_USAGE _eUsage);

	CMesh* CreatePlaneMesh(const wstring& _strKey, UINT _iXFace, UINT _iZFace, D3D11_PRIMITIVE_TOPOLOGY _eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void DeleteCloneMtrl(CMaterial* _pMtrl);
	CSound* LoadSound(const wstring & _strKey, const wstring & _strFilePath, bool _bIs3D = false);

private:
	void SaveResource(FILE* _pFile);
	void LoadResource(FILE* _pFile);

public:
	CMeshData* LoadFBX(const wstring& _strPath);
	CMeshData* LoadMeshData(const wstring& _strPath);

	template<typename T>
	CRes* Load(const wstring& _strKey, const wstring& _strFilePath = L"");

	friend class CListDlg;
	friend class CSaveLoadMgr;

private:
	static CResMgr* pInst;

public:
	static CResMgr* GetInst()
	{
		if (pInst == nullptr)
			pInst = new CResMgr;

		return pInst;
	}

	void DistroyInst() { SAFE_DELETE(pInst); }

	CResMgr();
	~CResMgr();

};

template<typename T>
inline CRes * CResMgr::Load(const wstring & _strKey, const wstring & _strFilePath)
{
	const type_info& info = typeid(T);

	if (&info == &typeid(CMesh))
	{
		return (CRes*)FindMesh(_strKey);
	}	
	else if (&info == &typeid(CTexture))
	{
		CTexture* pTex = FindTexture(_strKey);

		if (NULL != pTex)
			return pTex;

		wstring strFullPath = CPathMgr::GetResPath();
		strFullPath += _strFilePath;
		if (_strFilePath.empty())
			return nullptr;

		pTex = CTexture::Create(strFullPath);

		if (NULL == pTex)
			return NULL;

		pTex->SetKey(_strKey);
		pTex->SetPath(_strFilePath);
		m_mapTex.insert(make_pair(_strKey, pTex));

		m_bChange = true;

		return (CRes*)pTex;
	}
	else if (&info == &typeid(CPrefab))
	{
		return (CRes*)FindPrefab(_strKey);
	}
	else if (&info == &typeid(CSound))
	{
		CSound* pSound = FindSound(_strKey);

		if (NULL != pSound)
			return pSound;

		wstring strFullPath = CPathMgr::GetResPath();
		strFullPath += _strFilePath;

		pSound = CSound::Create(strFullPath);

		if (NULL == pSound)
			return NULL;

		pSound->SetKey(_strKey);
		pSound->SetPath(_strFilePath);
		m_mapSound.insert(make_pair(_strKey, pSound));

		m_bChange = true;

		return (CRes*)pSound;
	}

	return NULL;
}
