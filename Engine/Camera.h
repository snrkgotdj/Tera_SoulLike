#pragma once
#include "Component.h"

#include "Frustum.h"

union uInstID
{
	struct {
		unsigned short sMesh;
		unsigned short sMtrlNum;
		UINT iMtrl;
	};
	ULONG64 llID;
};

struct tInstObj
{
	CGameObject* pObj;
	UINT iMtrlIdx;
};

class CLayer;
class CCamFilter;
class CMaterial;

class CCamera :
	public CComponent
{
private:
	CFrustum							m_Frustum;
	CCamFilter*							m_pFilter;
	CCamFilter*							m_pPrevFiter;
	CGameObject*						m_pSkyBoxObj;
	CCamFilter*							m_pHDRFilter;

	// Instancing Group
	map<ULONG64, vector<tInstObj>*>		m_mapInstGroup_F;
	map<ULONG64, vector<tInstObj>*>		m_mapInstGroup_D;
	map<ULONG64, vector<CGameObject*>*> m_mapInstGroup_S;
	map<ULONG64, vector<tInstObj>*>		m_mapInstGroup_P;

	map<INT_PTR, vector<tInstObj>>		m_mapSingleObj;
	vector<CGameObject*>				m_vecSingleObj_S;
	vector<CGameObject*>				m_vecToolObj;
	vector<CGameObject*>				m_vecColObj;
	vector<CGameObject*>				m_vecParticleObj;

	Matrix								m_matView;
	Matrix								m_matProj;
	Matrix								m_matLookAt;
	
	bool								m_bPerspective;
	bool								m_bSkyBoxRender;

	// 원근 
	float								m_fFOV;	

	// 직교
	float								m_fWidth;
	float								m_fHeight;
	float								m_fScale;

	// 공통
	float								m_fNear;
	float								m_fFar;

	UINT								m_iLayerCheck;
	UINT								m_iBeforeLayer;

	CAMERA_TYPE							m_eCamType;

	bool								m_bLookAt;

public:
	virtual int Update();	
	virtual int FinalUpdate();

	void RenderForward(bool _bIsUI = false);
	void RenderDeferred();
	void RenderFilter();
	void RenderToolObj();
	void RenderCollider();
	void RenderSkyBox();
	void Render_ShadowMap(CMaterial* _pMtrl);
	void RenderPostEffect();
	bool RenderHDR();
	void SetSkyboxRender(bool _bBool) { m_bSkyBoxRender = _bBool; }

public:
	void LayerRenderCheck(const wstring& _strLayerName);
	void SetAllLayerCheck() { m_iLayerCheck = 0xffffffff; };
	void SetAllLayerUnCheck() { m_iBeforeLayer = m_iLayerCheck;  m_iLayerCheck = 0x00000000; }
	void RevertLayer() { m_iLayerCheck = m_iBeforeLayer; m_bSkyBoxRender = true; }
	void SetWidth(float _fWidth) { m_fWidth = _fWidth; }
	void SetHeight(float _fHeight) { m_fHeight = _fHeight; }
	void SetScale(float _fScale) { m_fScale = _fScale; }
	void SetNear(float _fNear) { m_fNear = m_fNear; }
	void SetFar(float _fFar) { m_fFar = _fFar; }

	float GetFar() { return m_fFar; }
	float GetNear() { return m_fNear; }
	float GetWidth() { return m_fWidth; }
	float GetHeght() { return m_fHeight; }
	float GetScale() { return m_fScale; }
	const Matrix& GetProjMat() { return m_matProj; }
	const Matrix& GetViewMat() { return m_matView; }
	void Reset();

	void AddToolObj(CGameObject* _pObj) { m_vecToolObj.push_back(_pObj); }
	bool HasFilter() { return m_pFilter == NULL ? false : true; };
	void SetFilter(CCamFilter* _pFilter); 
	void SetPrevFilter() { m_pFilter = m_pPrevFiter; }
	void SetHDRFilter(CCamFilter* _pHDRFilter) { m_pHDRFilter = _pHDRFilter; }
	CCamFilter* GetFilter() { return m_pFilter; }

	void SetPerspective(bool _bTrue) { m_bPerspective = _bTrue; }
	bool IsPerspective() { return m_bPerspective; };

	tRay GetRay();

	Vec2 Get2DWorldPosFromWindowPos(const Vec2& _vWindowPos, const Vec2& _vResolution);

	void SetCameraType(const CAMERA_TYPE& _eType) { m_eCamType = _eType; }
	const CAMERA_TYPE& GetCameraType() { return m_eCamType; }

	void CreateSkyBox();
	CGameObject* GetObjectByPicking(CLayer* _pLayer);

	void SetLookAt(bool _bTrue) { m_bLookAt = _bTrue; }
	void SetLookAtMat(const Matrix& _matView) { m_matLookAt = _matView; }

private:
	void UpdateInstancing();	
	void UpdateInstancing_shadow();
	void CreateViewMat(const CTransform* _pTrans);
	void CreateProjMat();

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	virtual CCamera* Clone() { return new CCamera(*this); }
	
public:
	CCamera();
	virtual ~CCamera();

	friend class CCameraDlg;
	friend class CLight;
	friend class CRenderMgr;
};

