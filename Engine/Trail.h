#pragma once
#include "Component.h"

#include "ResPtr.h"

class CMesh;
class CTexture;
class CMaterial;

class CTrail :
	public CComponent
{
private:
	CResPtr<CMaterial>	m_pMtrl;
	Vec3				m_arrPos[2];
	CMesh*				m_pMesh;

	list<Matrix>		m_listMat;
	UINT				m_iMaxCount;

	float				m_fAddTime;
	float				m_fCurTime;
	float				m_fRenewTime;

	// Åø
	CGameObject*		m_arrToolObj[2];

	bool				m_bActive;
	float				m_fDT;

public:
	virtual void Start();
	virtual int Update();
	virtual int DelayUpdate();
	virtual void Render();

public:
	void SetRenewTime(float _fTime) { m_fRenewTime = _fTime; }
	void SetPos(UINT _iIdx, const Vec3& _vPos) { m_arrPos[_iIdx] = _vPos; }
	const Vec3& GetPos(UINT _iIdx) { return m_arrPos[_iIdx]; }
	void ToolRender();
	void SetActive(bool _bActive) { m_bActive = _bActive; }
	void SetTrailTexture(CTexture* _pTrailTex);
	void SetTrailMtrl(CMaterial* _pMtrl) { m_pMtrl = _pMtrl; }
	void SetMaxCount(int _iMaxTrail) { m_iMaxCount = _iMaxTrail; }

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CTrail);

public:
	CTrail();
	CTrail(const CTrail& _Other);
	virtual ~CTrail();
};

