#pragma once

#include "ResPtr.h"

class CSRT;
class CTexture;

class CMRT
{
public:
	static UINT			g_iDepthCount;

private:
	wstring				m_strKey;
	UINT				m_iRTCount;
	CSRT*				m_arrTarget[8];
	CResPtr<CTexture>	m_pDepthStencilTex;

	Vec2				m_vResolution;

public:
	static CMRT* Create(CSRT* _pTargets[8]);
	const wstring& GetKey() { return m_strKey; }

	void OMSet();
	void OMSet(UINT _iIdx);
	void OMSet(UINT* _piIdx, UINT _iIdxSize);
	
	void Clear(Vec4& _vCol = Vec4(0.5f, 0.5f, 0.5f, 0.f));
	void CreateDepthStencilBuffer(Vec2 _vResolution);

private:
	void SetRenderTargets(CSRT* _pTargets[8]);
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }

public:
	CMRT();
	~CMRT();

	friend class CRenderMgr;
};

