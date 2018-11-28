#pragma once

#include "Res.h"
#include "ResPtr.h"
#include "FBXLoader.h"

class CMaterial;
class CMesh;
class CGameObject;

class CMeshData :
	public CRes
{
private:
	CResPtr<CMesh>				m_pMesh;
	vector<CResPtr<CMaterial>>	m_vecMtrl;

	// Animation

public:
	static CMeshData* LoadFromFBX(const wstring& _strFilePath);
	static CMeshData* LoadFromMeshData(const wstring& _strFilePath);

	void SaveMeshData();

	CGameObject* Instantiate();

public:
	CMeshData();
	virtual ~CMeshData();
};