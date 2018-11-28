#include "BlendState.h"

#include "Device.h"

CBlendState::CBlendState()
	: m_tDesc{}
	, m_bIndependent(false)
{
}

CBlendState::~CBlendState()
{
	SAFE_RELEASE(m_pBlendState);
}

void CBlendState::AddTargetBlendDesc(UINT _iIdx, const D3D11_RENDER_TARGET_BLEND_DESC & _TargetDesc)
{
	m_tDesc.RenderTarget[_iIdx] = _TargetDesc;
}

void CBlendState::CreateBlendState()
{
	m_tDesc.IndependentBlendEnable = m_bIndependent;

	if (FAILED(DEVICE->CreateBlendState(&m_tDesc, &m_pBlendState)))
	{
		assert(NULL);
	}
}

void CBlendState::UpdateData()
{
	CONTEXT->OMSetBlendState(m_pBlendState, NULL, 0xffffffff);
}
