#pragma once
#include "Res.h"
// https://github.com/Microsoft/DirectXTex

class CTexture :
	public CRes
{
private:
	ScratchImage*				m_pImage;
	ID3D11Texture2D*			m_pTex2D;

	ID3D11ShaderResourceView*	m_pSRV;
	ID3D11RenderTargetView*		m_pRTV;
	ID3D11DepthStencilView*		m_pDSV;

	D3D11_TEXTURE2D_DESC		m_tDesc;

public:
	static CTexture* Create(const wstring& _strFileName);
	static CTexture* Create(UINT _iWidth, UINT _iHeight, UINT _iBindFlag, DXGI_FORMAT _eFormat, D3D11_USAGE _eUsage = D3D11_USAGE::D3D11_USAGE_DEFAULT);
	static CTexture* Create(ID3D11Texture2D* _pTex2D, UINT _iBindFlag, DXGI_FORMAT _eFormat);
	static CTexture* Create(vector<CTexture*>& _vecTex, D3D11_USAGE _eUsage);
	ID3D11ShaderResourceView* GetShaerResourceView() { return m_pSRV; }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_pRTV; }
	ID3D11DepthStencilView* GetDepthStencilView() { return m_pDSV; }

	bool IsDynamic();
	void ChangeDynamic();
	float GetWidth() { return (float)m_tDesc.Width; }
	float GetHeight() { return (float)m_tDesc.Height;; }
	DXGI_FORMAT GetFormat() { return m_pImage->GetMetadata().format; }
	void* GetPixel() { return m_pImage->GetPixels(); }
	void SaveTexture(const wstring& _strPath);
	void Reset();
	void Resize(UINT _iWidth, UINT _iHeight);

public:	
	void UpdateData(UINT _iRegister, UINT _iTiming);
	static void ClearData(UINT _iRegister, UINT _iTiming);
	void SetData(void* _pSrc, size_t _size);
	
public:
	CTexture();
	virtual ~CTexture();
};


