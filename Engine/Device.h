#pragma once

#include "global.h"

class CDevice
{
	SINGLE(CDevice)

private:	
	ID3D11Device*			m_pDevice;	// 디바이스 - memory
	ID3D11DeviceContext*	m_pContext; // 컨텍스트 - rendering

	UINT					m_iMSLev; // 멀티 샘플링 레벨

	// 상수버퍼
	map<wstring, CBUFFER>	m_mapConstBuffer;
	ID3D11RasterizerState*  m_arrRasterizer[(UINT)RASTERIZE_TYPE::RT_END];

	ID3D11ShaderResourceView* m_pRandomSRV;

public:
	int init(HWND _hWnd, bool _bWindow);

	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetContext() { return m_pContext; }
	ID3D11Buffer* CreateIdxBuffer(UINT _iIdxCount, UINT _iIdxSize, void* _pIdxSysMem);

private:
	int CreateRasterizerState();
	int CreateRandomTexture();


public:
	const CBUFFER* FindConstBuffer(const wstring& _strKey);
	int CreateConstBuffer(const wstring& _strKey, UINT _iSize, D3D11_USAGE _eUsage, UINT _iRegister);
	void SetRasterizer(RASTERIZE_TYPE _eType) { m_pContext->RSSetState(m_arrRasterizer[(UINT)_eType]); }
	void SetViewPort(Vec2 _vResolution);
};

