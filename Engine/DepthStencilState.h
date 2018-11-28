#pragma once

#include "global.h"

class CDepthStencilState
{
private:
	ID3D11DepthStencilState*	m_pState;
	D3D11_DEPTH_STENCIL_DESC	m_tDesc;

public:
	static CDepthStencilState* Create(bool _bDepthEable, D3D11_DEPTH_WRITE_MASK _eMask
		, D3D11_COMPARISON_FUNC _eFunc, bool _bStencilEable = false
		, D3D11_DEPTH_STENCILOP_DESC _front = {}, D3D11_DEPTH_STENCILOP_DESC _back = {}
		, UINT _iStencilReadMask = 0xff, UINT _iStencilWriteMask = 0xff);

public:
	void UpdateData(UINT _iStencil = 0);

public:
	CDepthStencilState();
	~CDepthStencilState();
};

