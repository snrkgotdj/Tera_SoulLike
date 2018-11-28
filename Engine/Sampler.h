#pragma once

#include "global.h"

class CSampler
{
private:
	D3D11_SAMPLER_DESC	m_tDesc;
	ID3D11SamplerState* m_pState;
	UINT				m_iRegister;

public:
	static CSampler* Create(D3D11_FILTER _eFilter
		, D3D11_TEXTURE_ADDRESS_MODE _eUMode
		, D3D11_TEXTURE_ADDRESS_MODE _eVMode
		, UINT _iRegister);

	void UpdateData(UINT _iTiming);

public:
	CSampler();
	~CSampler();
};

