#include "Sampler.h"

#include "Device.h"

CSampler::CSampler()
	: m_tDesc{}
	, m_pState(NULL)
	, m_iRegister(0)
{
}

CSampler::~CSampler()
{
	SAFE_RELEASE(m_pState);
}

CSampler * CSampler::Create(D3D11_FILTER _eFilter, D3D11_TEXTURE_ADDRESS_MODE _eUMode
	, D3D11_TEXTURE_ADDRESS_MODE _eVMode
	, UINT _iRegister)
{
	D3D11_SAMPLER_DESC tDesc = {};
	tDesc.Filter = _eFilter;
	tDesc.AddressU = _eUMode;
	tDesc.AddressV = _eVMode;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	ID3D11SamplerState* pState = NULL;
	if (FAILED(DEVICE->CreateSamplerState(&tDesc, &pState)))
	{
		assert(NULL);
		return NULL;
	}

	CSampler* pSampler = new CSampler;
	pSampler->m_iRegister = _iRegister;
	pSampler->m_pState = pState;
	pSampler->m_tDesc = tDesc;

	return pSampler;
}

void CSampler::UpdateData(UINT _iTiming)
{
	if (_iTiming & (UINT)SHADER_TYPE::ST_VERTEX)
		CONTEXT->VSSetSamplers(m_iRegister, 1, &m_pState);
	if (_iTiming & (UINT)SHADER_TYPE::ST_HULL)
		CONTEXT->HSSetSamplers(m_iRegister, 1, &m_pState);
	if (_iTiming & (UINT)SHADER_TYPE::ST_DOMAIN)
		CONTEXT->DSSetSamplers(m_iRegister, 1, &m_pState);
	if (_iTiming & (UINT)SHADER_TYPE::ST_GEOMETRY)
		CONTEXT->GSSetSamplers(m_iRegister, 1, &m_pState);
	if (_iTiming & (UINT)SHADER_TYPE::ST_COMPUTE)
		CONTEXT->CSSetSamplers(m_iRegister, 1, &m_pState);
	if (_iTiming & (UINT)SHADER_TYPE::ST_PIXEL)
		CONTEXT->PSSetSamplers(m_iRegister, 1, &m_pState);
}
