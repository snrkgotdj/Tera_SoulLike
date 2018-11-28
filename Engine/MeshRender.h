#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "CamFilter.h"

class CMesh;
class CMaterial;


class CMeshRender :
	public CComponent
{
private:
	CResPtr<CMesh>				m_pMesh;
	vector<CResPtr<CMaterial>>	m_vecMtrl;
	RASTERIZE_TYPE				m_eRT;

	CResPtr<CMaterial>			m_pTrailMtrl;

	bool						m_bShadow;
	bool						m_bPostEffect;

public:
	virtual int Update() { return 0; };
	virtual void Render();
	void Render(UINT _iMtrlIdx);

public:	
	void SetMesh(CMesh* _pMesh);
	void SetMaterial(CMaterial* _pMaterial, UINT _iIdx = 0);
	void SetRSMode(RASTERIZE_TYPE _eMode) { m_eRT = _eMode; }
	void SetCastShadow(bool _bTrue) { m_bShadow = _bTrue; }
	bool CastShadow() { return m_bShadow; }
	void MaterialResize(UINT _iSize) { m_vecMtrl.resize(_iSize); }
	void SetPostEffect(bool _bTrue) { m_bPostEffect = _bTrue; }

public:
	CMesh* GetMesh() { return m_pMesh; };
	CMaterial* GetSharedMaterial(UINT _iIdx = 0) { return m_vecMtrl[_iIdx]; }
	CMaterial* GetMaterial(UINT _iIdx = 0);
	CMaterial* GetUIMaterial(UINT _iIdx = 0);
	vector<CResPtr<CMaterial>>& GetMtrlVec() { return m_vecMtrl; }
	RASTERIZE_TYPE GetRSType() { return m_eRT; }
	ULONG64 GetInstID(UINT _iIdx = 0);
	UINT GetMaterialSize() { return (UINT)m_vecMtrl.size(); }
	bool IsShadowCast() { return m_bShadow; }
	bool IsPostEffect() { return m_bPostEffect; }
	CMaterial* GetTrailMaterial() { return m_pTrailMtrl; }

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

	virtual CMeshRender* Clone() { return new CMeshRender(*this); }

public:
	CMeshRender();
	virtual ~CMeshRender();
};

