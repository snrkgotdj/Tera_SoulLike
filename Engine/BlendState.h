#pragma once

#include "global.h"

class CBlendState
{
private:
	D3D11_BLEND_DESC	m_tDesc;
	ID3D11BlendState*	m_pBlendState;
	bool				m_bIndependent;

public:
	void SetIndependent(bool _bTrue) { m_bIndependent = _bTrue; }
	void AddTargetBlendDesc(UINT _iIdx, const D3D11_RENDER_TARGET_BLEND_DESC& _TargetDesc);
	void CreateBlendState();

	void UpdateData();


public:
	CBlendState();
	~CBlendState();
};

