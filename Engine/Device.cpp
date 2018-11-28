#include "Device.h"

#include "ResMgr.h"
#include "RenderMgr.h"
#include "SRT.h"

CDevice::CDevice()
	: m_pDevice(NULL)
	, m_pContext(NULL)
	, m_iMSLev(4)
	, m_pRandomSRV(NULL)
{
}

CDevice::~CDevice()
{
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

	map<wstring, CBUFFER>::iterator iter = m_mapConstBuffer.begin();
	for (; iter != m_mapConstBuffer.end(); ++iter)
	{
		SAFE_RELEASE(iter->second.pBuffer);
	}

	for (int i = 0; i < (UINT)RASTERIZE_TYPE::RT_END; ++i)
	{
		SAFE_RELEASE(m_arrRasterizer[i]);
	}

	SAFE_RELEASE(m_pRandomSRV);
}

int CDevice::init(HWND _hWnd, bool _bWindow)
{
	int iFlag = 0;
#ifdef _DEBUG
	iFlag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL eFeatureLv = (D3D_FEATURE_LEVEL)0;

	if (FAILED(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE
		, NULL, iFlag, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &eFeatureLv, &m_pContext)))
	{
		return RET_FAILED;
	}
	
	// 멀티샘플 체크
	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_iMSLev);
	if (0 > m_iMSLev)
	{
		return RET_FAILED;
	}

	// Render Manager 초기화
	CRenderMgr::GetInst()->Init(_hWnd, _bWindow);

	// View Port 설정
	SetViewPort(CRenderMgr::GetInst()->GetWindowResolution());

	// Rasterizer state
	CreateRasterizerState();

	CreateRandomTexture();


	return RET_SUCCESS;
}

ID3D11Buffer * CDevice::CreateIdxBuffer(UINT _iIdxCount, UINT _iIdxSize, void* _pIdxSysMem)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	memset(&tBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = _iIdxSize* _iIdxCount;

	tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tSub.pSysMem = _pIdxSysMem;

	ID3D11Buffer* pIB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pIB)))
	{
		assert(NULL);
	}
	return pIB;
}

int CDevice::CreateRasterizerState()
{
	m_arrRasterizer[(UINT)RASTERIZE_TYPE::RT_BACK] = NULL;

	D3D11_RASTERIZER_DESC tDesc = {};

	tDesc.FillMode = D3D11_FILL_WIREFRAME;
	tDesc.CullMode = D3D11_CULL_NONE;
	tDesc.DepthClipEnable = true;

	m_pDevice->CreateRasterizerState(&tDesc, &m_arrRasterizer[(UINT)RASTERIZE_TYPE::RT_WIREFRAME]);

	tDesc.FillMode = D3D11_FILL_SOLID;
	tDesc.CullMode = D3D11_CULL_FRONT;
	tDesc.DepthClipEnable = true;

	m_pDevice->CreateRasterizerState(&tDesc, &m_arrRasterizer[(UINT)RASTERIZE_TYPE::RT_FRONT]);

	tDesc.FillMode = D3D11_FILL_SOLID;
	tDesc.CullMode = D3D11_CULL_NONE;
	tDesc.DepthClipEnable = true;

	m_pDevice->CreateRasterizerState(&tDesc, &m_arrRasterizer[(UINT)RASTERIZE_TYPE::RT_NONE]);

	return 0;
}


int CDevice::CreateRandomTexture()
{
	XMFLOAT4 randomValue[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValue[i].x = getRandNum_float(-1.f, 1.f);
		randomValue[i].y = getRandNum_float(-1.f, 1.f);
		randomValue[i].z = getRandNum_float(-1.f, 1.f);
		randomValue[i].w = getRandNum_float(-1.f, 1.f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValue;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	// 텍스쳐 생성

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;
	HRESULT hr = m_pDevice->CreateTexture1D(&texDesc, &initData, &randomTex);

	if (hr != S_OK)
		assert(NULL);

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	m_pDevice->CreateShaderResourceView(randomTex, &viewDesc, &m_pRandomSRV);

	SAFE_RELEASE(randomTex);

	return RET_SUCCESS;
}

const CBUFFER * CDevice::FindConstBuffer(const wstring & _strKey)
{
	map<wstring, CBUFFER>::iterator iter = m_mapConstBuffer.find(_strKey);

	if (iter == m_mapConstBuffer.end())
		return NULL;

	return &iter->second;
}

int CDevice::CreateConstBuffer(const wstring & _strKey, UINT _iSize, D3D11_USAGE _eUsage, UINT _iRegister)
{
	CBUFFER buffer = {};	

	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = _iSize;
	tBufferDesc.Usage = _eUsage;
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if(tBufferDesc.Usage == D3D11_USAGE_DYNAMIC)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &buffer.pBuffer)))
	{
		return RET_FAILED;
	}

	buffer.iRegister = _iRegister;
	buffer.iSize = _iSize;

	m_mapConstBuffer.insert(make_pair(_strKey, buffer));

	return 0;
}

void CDevice::SetViewPort(Vec2 _vResolution)
{
	// View Port 설정
	D3D11_VIEWPORT tViewPortDesc = {};

	tViewPortDesc.Width = _vResolution.x;
	tViewPortDesc.Height = _vResolution.y;

	tViewPortDesc.MinDepth = 0;
	tViewPortDesc.MaxDepth = 1;

	// 좌상단
	tViewPortDesc.TopLeftX = 0;
	tViewPortDesc.TopLeftY = 0;

	m_pContext->RSSetViewports(1, &tViewPortDesc);
}
