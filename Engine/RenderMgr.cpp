#include "RenderMgr.h"

#include "Sampler.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "SRT.h"
#include "MRT.h"
#include "CamFilter.h"

#include "TimeMgr.h"
#include "KeyMgr.h"
#include "ResMgr.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "GameObject.h"
#include "Light.h"
#include "Text.h"
#include "Camera.h"
#include "Material.h"
#include "Texture.h"

#include "Device.h"

RENDER_MODE CRenderMgr::m_eMode = RENDER_MODE::FORWARD;

CRenderMgr* CRenderMgr::pInst = nullptr;

CRenderMgr::CRenderMgr()
	: m_pSwapChain(NULL)
	, m_hWnd(NULL)
	, m_pLMFilter(NULL)
	, m_bWindow(false)
	, m_vResolution()
	, m_bDebugMod(false)
{
}


CRenderMgr::~CRenderMgr()
{
	SAFE_RELEASE(m_pSwapChain);

	Safe_Delete_Map(m_mapSampler);
	Safe_Delete_Map(m_mapBlendState);
	Safe_Delete_Map(m_mapDepthStencilState);
	Safe_Delete_Map(m_mapRenderTarget);
	Safe_Delete_Map(m_mapFilter);

	for (UINT i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		SAFE_DELETE(m_arrMRT[i]);
	}
}

int CRenderMgr::Init(HWND _hWnd, bool _bWindow)
{
	m_hWnd = _hWnd;
	m_bWindow = _bWindow;

	RECT rt = {};
	GetWindowRect(m_hWnd, &rt);

	m_vResolution = Vec2((float)rt.right - (float)rt.left, (float)rt.bottom - (float)rt.top);

	// 스왚 체인 생성
	if (FAILED(CreateSwapChain()))
	{
		return RET_FAILED;
	}

	// RenderTargetView, DepthStencilView
	if (FAILED(CreateMRT()))
	{
		return RET_FAILED;
	}

	CreateSampler();
	CreateBlendState();
	CreateDepthStencilState();

	return RET_SUCCESS;
}


int CRenderMgr::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC tDesc = {};

	Vec2 vResolution = GetWindowResolution();

	tDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tDesc.BufferDesc.Width = (UINT)vResolution.x;
	tDesc.BufferDesc.Height = (UINT)vResolution.y;

	tDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	tDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 화면 갱신률
	tDesc.BufferDesc.RefreshRate.Numerator = 60;
	tDesc.BufferDesc.RefreshRate.Denominator = 1;

	// 버퍼 용도 지정
	tDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 창모드 설정
	tDesc.OutputWindow = m_hWnd;
	tDesc.Windowed = m_bWindow;

	// 멀티 샘플
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;

	// 출력 이후에 버퍼내용 저장할 필요 없음
	tDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 버퍼 개수
	tDesc.BufferCount = 1;

	IDXGIDevice* pDXGIDevice = NULL;
	IDXGIAdapter* pAdapter = NULL;
	IDXGIFactory* pFactory = NULL;

	DEVICE->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
	pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	if (FAILED(pFactory->CreateSwapChain(DEVICE, &tDesc, &m_pSwapChain)))
	{
		return RET_FAILED;
	}

	SAFE_RELEASE(pDXGIDevice);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pFactory);

	return RET_SUCCESS;
}

int CRenderMgr::CreateMRT()
{
	Vec2 vResoultion = GetWindowResolution();

	//=====================
	// Render Target 만들기
	//=====================
	// Swamcahin Target
	ID3D11Texture2D*	pBackBuffer = NULL;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	CSRT* pTarget = CSRT::Create(pBackBuffer, D3D11_BIND_RENDER_TARGET);
	AddRenderTarget(L"SWAPCHAIN", pTarget);

	// FORWARD Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AddRenderTarget(L"FORWARD", pTarget);

	// Diffuse Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AddRenderTarget(L"DIFFUSE", pTarget);

	// Position Target(View Space)
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT);
	AddRenderTarget(L"POSITION", pTarget);

	// Normal Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT);
	AddRenderTarget(L"NORMAL", pTarget);

	// Depth Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32G32B32A32_FLOAT);
	AddRenderTarget(L"DEPTH", pTarget);

	// Diffuse Light Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM);
	AddRenderTarget(L"LIGHT_DIFFUSE", pTarget);

	// Specular Light Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM);
	AddRenderTarget(L"LIGHT_SPECULAR", pTarget);

	// 8192
	// Dir Light Shadow Map Target
	pTarget = CSRT::Create(4096, 4096
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32_FLOAT);
	AddRenderTarget(L"DIR_SHADOWMAP", pTarget);

	// POST Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AddRenderTarget(L"POST", pTarget);

	// Velocity Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM);
	AddRenderTarget(L"VELOCITY", pTarget);

	// Emissive Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AddRenderTarget(L"Emissive", pTarget);

	// Specular Target
	pTarget = CSRT::Create((UINT)vResoultion.x, (UINT)vResoultion.y
		, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R16G16B16A16_FLOAT);
	AddRenderTarget(L"SPECULAR", pTarget);

	//===========
	// MRT 만들기
	//===========
	// OUTPUT MRT
	CSRT* pTargets[8] = { FindRenderTarget(L"SWAPCHAIN") , FindRenderTarget(L"FORWARD") , FindRenderTarget(L"POST"), nullptr, };
	CMRT* pMRT = CMRT::Create(pTargets);
	pMRT->CreateDepthStencilBuffer(CRenderMgr::GetInst()->GetWindowResolution());

	m_arrMRT[(UINT)MRT_TYPE::OUTPUT] = pMRT;
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();

	// DEFERRED MRT
	memset(pTargets, 0, sizeof(void*) * 8);
	pTargets[0] = FindRenderTarget(L"DIFFUSE");
	pTargets[1] = FindRenderTarget(L"POSITION");
	pTargets[2] = FindRenderTarget(L"NORMAL");
	pTargets[3] = FindRenderTarget(L"DEPTH");
	pTargets[4] = FindRenderTarget(L"SPECULAR");
	pTargets[5] = FindRenderTarget(L"VELOCITY");
	pTargets[6] = FindRenderTarget(L"Emissive");
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = CMRT::Create(pTargets);

	// Light MRT
	memset(pTargets, 0, sizeof(void*) * 8);
	pTargets[0] = FindRenderTarget(L"LIGHT_DIFFUSE");
	pTargets[1] = FindRenderTarget(L"LIGHT_SPECULAR");

	m_arrMRT[(UINT)MRT_TYPE::LIGHT] = CMRT::Create(pTargets);

	// Shadow MRT
	memset(pTargets, 0, sizeof(void*) * 8);
	pTargets[0] = FindRenderTarget(L"DIR_SHADOWMAP");

	m_arrMRT[(UINT)MRT_TYPE::SHADOW] = CMRT::Create(pTargets);
	m_arrMRT[(UINT)MRT_TYPE::SHADOW]->CreateDepthStencilBuffer(Vec2(4096, 4096));

	return RET_SUCCESS;
}

void CRenderMgr::UpdateLightBuffer()
{
	tLightArr tArr = {};
	UINT i = 0;
	for (auto& pObj : m_setLightObj)
	{
		if (false == pObj->IsActive())
			continue;

		tArr.arrInfo[i] = pObj->Light()->GetLightInfo();
		pObj->Light()->SetIdx(i);
		++i;
	}
	tArr.iLightCount = i;

	const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"LightArr");

	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &tArr, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
}

void CRenderMgr::ClearShaderResource()
{
	const UINT iResCount = (UINT)SHADER_PARAM::SP_TEX_END - (UINT)SHADER_PARAM::SP_TEX_0;
	ID3D11ShaderResourceView* ppSRV[iResCount] = {};
	UINT iStartSlot = g_SPRegister[(UINT)SHADER_PARAM::SP_TEX_0];

	CONTEXT->VSSetShaderResources(iStartSlot, iResCount, ppSRV);
	CONTEXT->HSSetShaderResources(iStartSlot, iResCount, ppSRV);
	CONTEXT->DSSetShaderResources(iStartSlot, iResCount, ppSRV);
	CONTEXT->GSSetShaderResources(iStartSlot, iResCount, ppSRV);
	CONTEXT->CSSetShaderResources(iStartSlot, iResCount, ppSRV);
	CONTEXT->PSSetShaderResources(iStartSlot, iResCount, ppSRV);
}

void CRenderMgr::CreateSampler()
{
	//================
	// Default Sampler
	//================
	CSampler* pSampler = CSampler::Create(D3D11_FILTER_ANISOTROPIC
		, D3D11_TEXTURE_ADDRESS_WRAP
		, D3D11_TEXTURE_ADDRESS_WRAP
		, 0);

	AddSampler(L"DefaultSampler", pSampler);

	pSampler = CSampler::Create(D3D11_FILTER_MIN_MAG_MIP_LINEAR
		, D3D11_TEXTURE_ADDRESS_WRAP
		, D3D11_TEXTURE_ADDRESS_WRAP
		, 1);
	AddSampler(L"Sampler_Linear", pSampler);
}

void CRenderMgr::CreateBlendState()
{
	CBlendState* pState = NULL;

	// Alpha Blend State
	pState = new CBlendState;
	D3D11_RENDER_TARGET_BLEND_DESC tTargetDesc = {};

	tTargetDesc.BlendEnable = true;
	tTargetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tTargetDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	tTargetDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	tTargetDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	tTargetDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pState->AddTargetBlendDesc(0, tTargetDesc);
	pState->CreateBlendState();

	AddBlendState(L"AlphaBlend", pState);

	// ONE Blend State(검은색 추출)
	pState = new CBlendState;
	tTargetDesc = D3D11_RENDER_TARGET_BLEND_DESC{};

	tTargetDesc.BlendEnable = true;
	tTargetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlend = D3D11_BLEND_ONE;
	tTargetDesc.DestBlend = D3D11_BLEND_ONE;

	tTargetDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	tTargetDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	tTargetDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pState->AddTargetBlendDesc(0, tTargetDesc);
	pState->CreateBlendState();

	AddBlendState(L"OneBlend", pState);

	// Color Write Disable
	pState = new CBlendState;
	tTargetDesc = D3D11_RENDER_TARGET_BLEND_DESC{};
	tTargetDesc.RenderTargetWriteMask = 0;

	pState->AddTargetBlendDesc(0, tTargetDesc);

	pState->CreateBlendState();
	AddBlendState(L"ColorWriteDisable", pState);

	// Additive Belnd state
	pState = new CBlendState;
	tTargetDesc = D3D11_RENDER_TARGET_BLEND_DESC{};

	tTargetDesc.BlendEnable = true;
	tTargetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tTargetDesc.DestBlend = D3D11_BLEND_ONE;

	tTargetDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tTargetDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	tTargetDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	tTargetDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	pState->AddTargetBlendDesc(0, tTargetDesc);
	pState->CreateBlendState();

	AddBlendState(L"Additive Blend", pState);

}

void CRenderMgr::CreateDepthStencilState()
{
	CDepthStencilState* pState = NULL;

	pState = CDepthStencilState::Create(true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	AddDepthStencilState(L"LESS_EQUAL", pState);

	pState = CDepthStencilState::Create(true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS_EQUAL);
	AddDepthStencilState(L"LESS_EQUAL2", pState);

	pState = CDepthStencilState::Create(false, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_ALWAYS);
	AddDepthStencilState(L"ALWAYS", pState);

	pState = CDepthStencilState::Create(false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS);
	AddDepthStencilState(L"ALWAYS_NONE", pState);

	pState = CDepthStencilState::Create(false, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_ALWAYS);
	AddDepthStencilState(L"DISABLE", pState);

	pState = CDepthStencilState::Create(true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_ALWAYS);
	AddDepthStencilState(L"NoDepthWrite", pState);

	//====================
	// Ligth MRT 전용
	//====================
	// 뒷면보다 앞에 있는 놈 걸러내기
	D3D11_DEPTH_STENCILOP_DESC tDepthStencil = {};

	tDepthStencil.StencilFunc = D3D11_COMPARISON_ALWAYS;
	tDepthStencil.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencil.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencil.StencilPassOp = D3D11_STENCIL_OP_REPLACE;

	pState = CDepthStencilState::Create(true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_GREATER
		, true, tDepthStencil, tDepthStencil);
	AddDepthStencilState(L"BEFORE_BACK", pState);

	// BEFORE_BACK 통과 기준 && 앞면보다 뒤에 있는 놈
	tDepthStencil.StencilFunc = D3D11_COMPARISON_EQUAL;
	tDepthStencil.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	tDepthStencil.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	tDepthStencil.StencilPassOp = D3D11_STENCIL_OP_KEEP;

	pState = CDepthStencilState::Create(true, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_LESS
		, true, tDepthStencil, tDepthStencil);
	AddDepthStencilState(L"AFTER_FRONT", pState);

	// 지정한 스탠실 값만 통과
	tDepthStencil.StencilFunc = D3D11_COMPARISON_EQUAL;
	tDepthStencil.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	tDepthStencil.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencil.StencilPassOp = D3D11_STENCIL_OP_ZERO;

	pState = CDepthStencilState::Create(false, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_ALWAYS
		, true, tDepthStencil, tDepthStencil);
	AddDepthStencilState(L"STENCIL_PASS", pState);
}

int CRenderMgr::AddFilter(const wstring & _strKey, CCamFilter * _pFilter)
{
	CCamFilter* pFilter = FindFilter(_strKey);

	if (NULL != pFilter)
		return RET_FAILED;

	m_mapFilter.insert(make_pair(_strKey, _pFilter));
	_pFilter->SetKey(_strKey);

	return RET_SUCCESS;
}

int CRenderMgr::AddSampler(const wstring & _strKey, CSampler * _pSampler)
{
	CSampler* pShader = FindSampler(_strKey);

	if (NULL != pShader)
		return RET_FAILED;

	m_mapSampler.insert(make_pair(_strKey, _pSampler));

	return RET_SUCCESS;
}

int CRenderMgr::AddBlendState(const wstring & _strKey, CBlendState * _pState)
{
	CBlendState* pState = FindBlendState(_strKey);

	if (NULL != pState)
		return RET_FAILED;

	m_mapBlendState.insert(make_pair(_strKey, _pState));

	return RET_SUCCESS;
}

int CRenderMgr::AddDepthStencilState(const wstring & _strKey, CDepthStencilState * _pState)
{
	CDepthStencilState* pState = FindDepthStencilState(_strKey);

	if (NULL != pState)
		return RET_FAILED;

	m_mapDepthStencilState.insert(make_pair(_strKey, _pState));

	return RET_SUCCESS;
}

int CRenderMgr::AddRenderTarget(const wstring & _strKey, CSRT * _pTarget)
{
	CSRT* pTarget = FindRenderTarget(_strKey);

	if (NULL != pTarget)
		return RET_FAILED;

	m_mapRenderTarget.insert(make_pair(_strKey, _pTarget));
	_pTarget->SetKey(_strKey);

	return RET_SUCCESS;
}

CCamFilter * CRenderMgr::FindFilter(const wstring & _strKey)
{
	map<wstring, CCamFilter*>::iterator iter = m_mapFilter.find(_strKey);

	if (iter == m_mapFilter.end())
		return NULL;

	return iter->second;
}

CSampler * CRenderMgr::FindSampler(const wstring & _strKey)
{
	map<wstring, CSampler*>::iterator iter = m_mapSampler.find(_strKey);

	if (iter == m_mapSampler.end())
		return NULL;

	return iter->second;
}

CBlendState * CRenderMgr::FindBlendState(const wstring & _strKey)
{
	map<wstring, CBlendState*>::iterator iter = m_mapBlendState.find(_strKey);

	if (iter == m_mapBlendState.end())
		return NULL;

	return iter->second;
}

CDepthStencilState * CRenderMgr::FindDepthStencilState(const wstring & _strKey)
{
	map<wstring, CDepthStencilState*>::iterator iter = m_mapDepthStencilState.find(_strKey);

	if (iter == m_mapDepthStencilState.end())
		return NULL;

	return iter->second;
}

CSRT * CRenderMgr::FindRenderTarget(const wstring & _strKey)
{
	map<wstring, CSRT*>::iterator iter = m_mapRenderTarget.find(_strKey);

	if (iter == m_mapRenderTarget.end())
		return NULL;

	return iter->second;
}

void CRenderMgr::AddLightObject(CGameObject * _pLightObj)
{
	m_setLightObj.insert(_pLightObj);
}

void CRenderMgr::AddTextObject(CGameObject * _pTextObj)
{
	m_listTextObj.push_back(_pTextObj);
}

void CRenderMgr::DeleteTextObject(CGameObject * _pTextObj)
{
	list<CGameObject*>::iterator iter = m_listTextObj.begin();
	for (; iter != m_listTextObj.end(); ++iter)
	{
		if ((*iter) == _pTextObj)
		{
			m_listTextObj.erase((iter));
			return;
		}
	}
}

void CRenderMgr::Render()
{
	// Target Clear
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->Clear(Vec4(0.f, 0.f, 0.f, 0.f));
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Clear(Vec4(0.f, 0.f, 0.f, 0.f));
	m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Clear(Vec4(0.f, 0.f, 0.f, 0.f));

	// 1. Tool 로 렌더링, Play 상태 렌더링
	if (CSceneMgr::GetInst()->IsPlayMode())
	{
		RenderPlayMode();
	}
	else
	{
		RenderToolMode();
	}

	list<CGameObject*>::iterator iter = m_listTextObj.begin();
	for (; iter != m_listTextObj.end(); ++iter)
	{
		(*iter)->Text()->Render();
	}

	CTimeMgr::GetInst()->render(NULL);

	m_pSwapChain->Present(0, 0);

	// 디버그 모드
	if (KEY_INPUT(KEY_TAB, STATE_TAB))
	{
		if (m_bDebugMod)
			m_bDebugMod = false;
		else
			m_bDebugMod = true;
	}
}

void CRenderMgr::RenderPlayMode()
{
	RenderMainCam();
	RenderUICam();
}


void CRenderMgr::RenderToolMode()
{
	// Shader Resource Clear
	ClearShaderResource();

	// 1. 지연렌더링 그리기
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	CGameObject* m_pCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
	CCamera* pCamCom = m_pCamObj->Camera();
	
	// 인스턴싱 데이터 업데이트
	pCamCom->UpdateInstancing();

	pCamCom->RenderDeferred();

	// 2. 빛 처리(Light Target)
	// Light 정보를 모아서 레지스터에 전달
	UpdateLightBuffer();
	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);

	for (auto& pObj : m_setLightObj)
	{
		if (false == pObj->IsActive())
			continue;

		CLight* pLight = pObj->Light();
		if (pLight->GetLightInfo().iLightType == (UINT)LIGHT_TYPE::DIR)
		{
			pLight->GetLightCam().UpdateInstancing_shadow();
			// Shadow Map
			m_arrMRT[(UINT)MRT_TYPE::SHADOW]->OMSet();
			m_arrMRT[(UINT)MRT_TYPE::SHADOW]->Clear(Vec4(0.f, 0.f, 0.f, 0.f));
			pLight->render_shadowmap();
		}

		m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();
		pLight->Render();
	}

	// 3. Diffuse Target 과 Light Target 블랜딩
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();
	m_pLMFilter->Render();

	// 스카이 박스 렌더
	pCamCom->RenderSkyBox();

	// 4. Forward Render
	pCamCom->RenderForward();

	for (auto& pObj : m_setLightObj)
	{
		if (false == pObj->IsActive())
			continue;

		CLight* pLight = pObj->Light();
		pLight->RangeRender();
	}

	// 충돌메쉬 렌더
	pCamCom->RenderCollider();
	
	UINT iIdx[2] = {0, 2};
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet(iIdx, 2);
	// 5. PostEffect Render
	pCamCom->RenderPostEffect();

	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet(0);

	if (pCamCom->HasFilter())
	{
		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		pCamCom->RenderFilter();
	}

	pCamCom->RenderToolObj();
}

void CRenderMgr::RenderMainCam()
{
	// Shader Resource Clear
	ClearShaderResource();

	// 1. Scene 에서 Camera를 가져온다.
	CGameObject* pCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);

	if (!pCamObj->IsActive())
		return;

	// 1. 지연렌더링 그리기
	m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->OMSet();

	CCamera* pCamCom = pCamObj->Camera();

	// 인스턴싱 데이터 업데이트
	pCamCom->UpdateInstancing();

	pCamCom->RenderDeferred();

	// 2. 빛 처리(Light Target)
	// Light 정보를 모아서 레지스터에 전달
	UpdateLightBuffer();
	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);

	for (auto& pObj : m_setLightObj)
	{
		if (false == pObj->IsActive())
			continue;

		CLight* pLight = pObj->Light();
		if (pLight->GetLightInfo().iLightType == (UINT)LIGHT_TYPE::DIR)
		{
			pLight->GetLightCam().UpdateInstancing_shadow();
			// Shadow Map
			m_arrMRT[(UINT)MRT_TYPE::SHADOW]->OMSet();
			m_arrMRT[(UINT)MRT_TYPE::SHADOW]->Clear(Vec4(0.f, 0.f, 0.f, 0.f));
			pLight->render_shadowmap();
		}

		m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->OMSet();
		pLight->Render();
	}

	// 3. Diffuse Target 과 Light Target 블랜딩
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();
	m_pLMFilter->Render();

	// 스카이 박스 렌더
	pCamCom->RenderSkyBox();

	// 4. 포워드 렌더링
	pCamCom->RenderForward();

	if (m_bDebugMod)
	{
		// 충돌메쉬 렌더
		pCamCom->RenderCollider();
	}

	UINT iIdx[2] = { 0, 2 };
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet(iIdx, 2);

	// 5. PostEffect Render
	pCamCom->RenderPostEffect();

	ClearShaderResource();

	iIdx[1] = 1;
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet(iIdx, 2);
	bool bHDR = pCamCom->RenderHDR();

	if (pCamCom->HasFilter())
	{
		if (bHDR)
		{
			CTexture* pTex = CResMgr::GetInst()->FindTexture(TARGET_FORWARD);
			CMaterial* pMtrl = pCamCom->GetFilter()->GetMaterial();
			pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
		}

		else
		{
			CTexture* pTex = CResMgr::GetInst()->FindTexture(TARGET_POST);
			pCamCom->GetFilter()->GetMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
		}
		m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet(0);
		pCamCom->RenderFilter();
	}
}

void CRenderMgr::RenderUICam()
{
	// Shader Resource Clear
	ClearShaderResource();

	// 1. Scene 에서 Camera를 가져온다.
	CGameObject* pCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::UI);

	if (NULL == pCamObj || !pCamObj->IsActive())
		return;

	CCamera* pCamCom = pCamObj->Camera();

	// 인스턴싱 데이터 업데이트
	pCamCom->UpdateInstancing();

	// 3. Diffuse Target 과 Light Target 블랜딩
	m_arrMRT[(UINT)MRT_TYPE::OUTPUT]->OMSet();

	// 4. 포워드 렌더링
	pCamCom->RenderForward(true);
}