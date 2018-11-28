#include "MRT.h"

#include "Device.h"
#include "RenderMgr.h"
#include "ResMgr.h"
#include "SRT.h"

UINT CMRT::g_iDepthCount = 0;

CMRT::CMRT()
	: m_arrTarget{}
	, m_pDepthStencilTex(NULL)
{
}

CMRT::~CMRT()
{
}

CMRT * CMRT::Create(CSRT * _pTargets[8])
{
	UINT i = 0;
	for (; i < 8; ++i)
	{
		if (NULL == _pTargets[i])
			break;
	}

	CMRT* pMRT = new CMRT;
	pMRT->SetRenderTargets(_pTargets);
	pMRT->m_iRTCount = i;

	return pMRT;
}


void CMRT::CreateDepthStencilBuffer(Vec2 _vResolution)
{
	wchar_t szName[50] = {};
	wsprintf(szName, L"DepthTex_%d", g_iDepthCount++);
	m_pDepthStencilTex = CResMgr::GetInst()->CreateTexture(szName
		, (UINT)_vResolution.x, (UINT)_vResolution.y
		, D3D11_BIND_DEPTH_STENCIL, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void CMRT::SetRenderTargets(CSRT * _pTargets[8])
{
	memcpy(m_arrTarget, _pTargets, sizeof(void*) * 8);

	UINT iIdx = 0;
	for (iIdx = 0; iIdx < 8; ++iIdx)
	{
		if (NULL != m_arrTarget[iIdx])
		{
			break;
		}
	}

	m_vResolution.x = m_arrTarget[iIdx]->GetTexture()->GetWidth();
	m_vResolution.y = m_arrTarget[iIdx]->GetTexture()->GetHeight();
}


void CMRT::OMSet()
{
	CDevice::GetInst()->SetViewPort(m_vResolution);

	ID3D11RenderTargetView* arrView[8] = {};
	for (UINT i = 0; i < m_iRTCount; ++i)
	{
		arrView[i] = m_arrTarget[i]->GetView();
	}

	if (m_pDepthStencilTex)
	{
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, m_pDepthStencilTex->GetDepthStencilView());
	}
	else
	{
		ID3D11DepthStencilView* pOut = NULL;
		CONTEXT->OMGetRenderTargets(0, NULL, &pOut);
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, pOut);
		SAFE_RELEASE(pOut)
	}
}

void CMRT::OMSet(UINT _iIdx)
{
	if (_iIdx >= m_iRTCount)
		assert(NULL);

	ID3D11RenderTargetView* arrView[8] = {};
	arrView[_iIdx] = m_arrTarget[_iIdx]->GetView();

	if (m_pDepthStencilTex)
	{
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, m_pDepthStencilTex->GetDepthStencilView());
	}
	else
	{
		ID3D11DepthStencilView* pOut = NULL;
		CONTEXT->OMGetRenderTargets(0, NULL, &pOut);
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, pOut);
		SAFE_RELEASE(pOut);
	}
}

void CMRT::OMSet(UINT * _piIdx, UINT _iIdxSize)
{
	if (_iIdxSize > m_iRTCount)
		assert(NULL);

	ID3D11RenderTargetView* arrView[8] = {};

	for (UINT i = 0; i < _iIdxSize; ++i)
	{
		arrView[_piIdx[i]] = m_arrTarget[_piIdx[i]]->GetView();
	}

	if (m_pDepthStencilTex)
	{
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, m_pDepthStencilTex->GetDepthStencilView());
	}
	else
	{
		ID3D11DepthStencilView* pOut = NULL;
		CONTEXT->OMGetRenderTargets(0, NULL, &pOut);
		CONTEXT->OMSetRenderTargets(m_iRTCount, arrView, pOut);
		SAFE_RELEASE(pOut);
	}
}

void CMRT::Clear(Vec4 & _vCol)
{
	float fCol[4] = { _vCol.x, _vCol.y, _vCol.z, _vCol.w };

	for (UINT i = 0; i < m_iRTCount; ++i)
	{
		m_arrTarget[i]->ClearTarget(fCol);
	}

	if (m_pDepthStencilTex)
	{
		CONTEXT->ClearDepthStencilView(m_pDepthStencilTex->GetDepthStencilView()
			, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}
