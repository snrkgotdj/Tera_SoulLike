#pragma once

#include "ResPtr.h"
#include "Texture.h"

class CSRT
{
private:
	static UINT		g_iRTCount;

private:
	wstring				m_strKey;
	CResPtr<CTexture>	m_pRTTex;

public:
	static CSRT* Create(UINT _iWidth, UINT _iHeight, UINT _iBindFlag, DXGI_FORMAT _eFormat, const wstring& _strKey = L"");
	static CSRT* Create(ID3D11Texture2D* _pTex2D, UINT _iBindFlag);

	void ClearTarget(float _fCol[4] = {});
	ID3D11RenderTargetView* GetView() { return m_pRTTex->GetRenderTargetView(); }
	const wstring& GetKey() { return m_strKey; }
	CTexture* GetTexture() { return m_pRTTex; }

private:
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }

public:
	CSRT();
	~CSRT();

	friend class CRenderMgr;
};