#pragma once

#include "global.h"

#include "ResPtr.h"
#include "Transform.h"

class CMesh;
class CMaterial;

class CCamFilter
{
protected:
	wstring				m_strKey;
	CResPtr<CMesh>		m_pMesh;
	CResPtr<CMaterial>	m_pMtrl;
	CTransform			m_Trans;

public:
	virtual void Render();

public:
	virtual int FinalUpdate();
	void RangeRender();

	void SetScale(const Vec3& _vScale) { m_Trans.SetLocalScale(_vScale); };
	void SetPos(const Vec3& _vPos) { m_Trans.SetLocalPos(_vPos); };
	void SetMesh(CMesh* _pMesh) { m_pMesh = _pMesh; };
	void SetMaterial(CMaterial* _pMtrl) { m_pMtrl = _pMtrl; }

	const wstring& GetKey() { return m_strKey; }
	CMaterial* GetMaterial() { return m_pMtrl; }

private:
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
	
	
public:
	CCamFilter* Clone() { return new CCamFilter(*this); }

	CCamFilter();
	virtual ~CCamFilter();

	friend class CRenderMgr;
};

