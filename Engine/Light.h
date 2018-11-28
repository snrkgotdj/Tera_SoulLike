#pragma once
#include "Component.h"
#include "Camera.h"
#include "ResPtr.h"

class CCamFilter;
class CTexture;
class CMaterial;

class CLight :
	public CComponent
{
public:
	static CCamFilter*	g_pLightFilter;

private:
	tLightInfo			m_tInfo;
	CCamFilter*			m_pVolumeFilter;
	UINT				m_iIdx; // Light Buffer 내에서의 인덱스
	float				m_fRange;
	CResPtr<CTexture>	m_pShadowMap;
	CResPtr<CMaterial>	m_pShadowMtrl;
	CCamera				m_LightCam;

	CResPtr<CMaterial>	m_pStd3DMtrl;

public:
	virtual int Update();
	virtual int FinalUpdate();
	virtual void Render();

public:
	void RangeRender();
	void render_shadowmap();

public:
	CCamera& GetLightCam() { return m_LightCam; }
	void SetLightInfo(const tLightInfo& _tLightInfo);
	const tLightInfo& GetLightInfo() { return m_tInfo; }
	void SetRange(float _fRange);
	void SetAngle(const Vec2& _vAngle) { m_tInfo.vSpotAngle = _vAngle; }
	void SetLightPos(Vec4 _vPos) { m_tInfo.vLightPos = _vPos; }
	void SetIdx(UINT _iIdx) { m_iIdx = _iIdx; }
	float GetRange() { return m_fRange; }

private:
	void SetColDiff(const Vec4& _vDiff) { m_tInfo.tCol.vDiff = _vDiff; }
	Vec4 GetColDiff() { return m_tInfo.tCol.vDiff; }
	void SetColSpec(const Vec4& _vSpec) { m_tInfo.tCol.vSpec = _vSpec; }
	Vec4 GetColSpec() { return m_tInfo.tCol.vSpec; }
	void SetColAmb(const Vec4& _vAmb) { m_tInfo.tCol.vAmb = _vAmb; }
	Vec4 GetColAmb() { return m_tInfo.tCol.vAmb; }


public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CLONE(CLight)

public:
	CLight();
	CLight(const tLightInfo& _info);
	virtual ~CLight();

	friend class CLightDlg;
};

