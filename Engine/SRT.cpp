#include "SRT.h"

#include "Device.h"
#include "ResMgr.h"

UINT CSRT::g_iRTCount = 0;

CSRT::CSRT()
	: m_pRTTex(NULL)
{
}

CSRT::~CSRT()
{
}

CSRT * CSRT::Create(UINT _iWidth, UINT _iHeight
	, UINT _iBindFlag, DXGI_FORMAT _eFormat, const wstring& _strKey)
{
	wchar_t szName[50] = {};
	
	if (_strKey.empty())
	{
		wsprintf(szName, L"RenderTarget_%d", g_iRTCount++);
	}
	else
	{
		wcscpy_s(szName, 50, _strKey.c_str());
	}

	CTexture* pTex = CResMgr::GetInst()->CreateTexture(szName, _iWidth, _iHeight
		, _iBindFlag, _eFormat);

	CSRT* pTarget = new CSRT;
	pTarget->m_pRTTex = pTex;

	return pTarget;
}

CSRT * CSRT::Create(ID3D11Texture2D * _pTex2D, UINT _iBindFlag)
{
	wchar_t szName[50] = {};
	wsprintf(szName, L"RenderTarget_%d", g_iRTCount++);

	CTexture* pTex = CResMgr::GetInst()->CreateTexture(szName, _pTex2D, _iBindFlag);

	CSRT* pTarget = new CSRT;
	pTarget->m_pRTTex = pTex;

	return pTarget;
}

void CSRT::ClearTarget(float _fCol[4])
{
	if (NULL == m_pRTTex->GetRenderTargetView())
		assert(NULL);

	CONTEXT->ClearRenderTargetView(m_pRTTex->GetRenderTargetView(), _fCol);
}