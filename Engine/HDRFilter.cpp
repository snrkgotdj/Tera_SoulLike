#include "HDRFilter.h"

#include "Device.h"
#include "RenderMgr.h"

#include "ResMgr.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

#include "ShaderMgr.h"
#include "Shader.h"

#include "KeyMgr.h"
#include "TimeMgr.h"

CHDRFilter::CHDRFilter()
	: m_pDownScale1DBuffer(NULL)
	, m_pDownScale1DUAV(NULL)
	, m_pDownScale1DSRV(NULL)
	, m_pAvgLumBuffer(NULL)
	, m_pAvgLumUAV(NULL)
	, m_pAvgLumSRV(NULL)
	, m_pPrevAvgLumBuffer(NULL)
	, m_pPrevAvgLumUAV(NULL)
	, m_pPrevAvgLumSRV(NULL)
	, m_pDownScaleCB(NULL)
	, m_pFinalPassCB(NULL)
	, m_pDownScaleRT(NULL)
	, m_pDownScaleSRV(NULL)
	, m_pDownScaleUAV(NULL)
	, m_pTempRT{ NULL, NULL }
	, m_pTempSRV{ NULL, NULL }
	, m_pTempUAV{ NULL, NULL }
	, m_pBloomRT(NULL)
	, m_pBloomSRV(NULL)
	, m_pBloomUAV(NULL)
	, m_pStateLinear(NULL)
	, m_pStatePoint(NULL)
	, m_iDownScaleGroups(0)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_fMiddleGrey(8.f)
	, m_fWhite(5.f)
	, m_fAdaptation(1.f)
	, m_fRenderTime(0.f)
	, m_fBloomThreshold(2.f)
	, m_fBloomScale(1.3f)
{
	Init();
}


CHDRFilter::~CHDRFilter()
{
	SAFE_RELEASE(m_pDownScale1DBuffer);
	SAFE_RELEASE(m_pDownScale1DUAV);
	SAFE_RELEASE(m_pDownScale1DSRV);
	SAFE_RELEASE(m_pAvgLumBuffer);
	SAFE_RELEASE(m_pAvgLumUAV);
	SAFE_RELEASE(m_pAvgLumSRV);
	SAFE_RELEASE(m_pPrevAvgLumBuffer);
	SAFE_RELEASE(m_pPrevAvgLumUAV);
	SAFE_RELEASE(m_pPrevAvgLumSRV);
	SAFE_RELEASE(m_pDownScaleCB);
	SAFE_RELEASE(m_pFinalPassCB);
	SAFE_RELEASE(m_pDownScaleRT);
	SAFE_RELEASE(m_pDownScaleSRV);
	SAFE_RELEASE(m_pDownScaleUAV);
	SAFE_RELEASE(m_pTempRT[0]);
	SAFE_RELEASE(m_pTempSRV[0]);
	SAFE_RELEASE(m_pTempUAV[0]);
	SAFE_RELEASE(m_pTempRT[1]);
	SAFE_RELEASE(m_pTempSRV[1]);
	SAFE_RELEASE(m_pTempUAV[1]);
	SAFE_RELEASE(m_pBloomRT);
	SAFE_RELEASE(m_pBloomSRV);
	SAFE_RELEASE(m_pBloomUAV);
	SAFE_RELEASE(m_pStateLinear);
	SAFE_RELEASE(m_pStatePoint);
}

void CHDRFilter::Init()
{
	Vec2 vResolution = CRenderMgr::GetInst()->GetWindowResolution();
	m_iWidth = (UINT)vResolution.x;
	m_iHeight = (UINT)vResolution.y;
	m_iDownScaleGroups = (UINT)((vResolution.x * vResolution.y) / (16.f * 1024.f));

	//=============
	// HDR
	//=============

	// 첫번째 버퍼(휘도 값을 계산할 때 중간 값을 저장하는 역할) 작성
	D3D11_BUFFER_DESC tBufferDesc = {};
	tBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tBufferDesc.StructureByteStride = sizeof(float);
	tBufferDesc.ByteWidth = m_iDownScaleGroups * tBufferDesc.StructureByteStride;
	tBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pDownScale1DBuffer)))
	{
		assert(NULL);
	}

	// UAV 디스크립터
	D3D11_UNORDERED_ACCESS_VIEW_DESC tUAVDecs = {};
	tUAVDecs.Format = DXGI_FORMAT_UNKNOWN;
	tUAVDecs.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	tUAVDecs.Buffer.NumElements = m_iDownScaleGroups;

	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pDownScale1DBuffer, &tUAVDecs, &m_pDownScale1DUAV)))
	{
		assert(NULL);
	}

	// SRV 디스크립터
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDecs = {};
	tSRVDecs.Format = DXGI_FORMAT_UNKNOWN;
	tSRVDecs.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	tSRVDecs.Buffer.NumElements = m_iDownScaleGroups;
	
	if (FAILED(DEVICE->CreateShaderResourceView(m_pDownScale1DBuffer, &tSRVDecs, &m_pDownScale1DSRV)))
	{
		assert(NULL);
	}

	// 두번째 버퍼(부동소수점 형태로 평균 휘도 값을 저장)
	tBufferDesc.ByteWidth = sizeof(float);

	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pAvgLumBuffer)))
	{
		assert(NULL);
	}

	// UVA 디스크립터
	tUAVDecs.Buffer.NumElements = 1;

	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pAvgLumBuffer, &tUAVDecs, &m_pAvgLumUAV)))
	{
		assert(NULL);
	}

	// SRV 디스크립터
	tSRVDecs.Buffer.NumElements = 1;

	if (FAILED(DEVICE->CreateShaderResourceView(m_pAvgLumBuffer, &tSRVDecs, &m_pAvgLumSRV)))
	{
		assert(NULL);
	}

	// 이전 두번째 버퍼
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pPrevAvgLumBuffer)))
	{
		assert(NULL);
	}

	// UVA 디스크립터
	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pPrevAvgLumBuffer, &tUAVDecs, &m_pPrevAvgLumUAV)))
	{
		assert(NULL);
	}

	// SRV 디스크립터
	if (FAILED(DEVICE->CreateShaderResourceView(m_pPrevAvgLumBuffer, &tSRVDecs, &m_pPrevAvgLumSRV)))
	{
		assert(NULL);
	}

	// 상수버퍼 2개 생성
	tBufferDesc = {};
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tBufferDesc.ByteWidth = sizeof(tDownScaleCB);

	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pDownScaleCB)))
	{
		assert(NULL);
	}

	tBufferDesc.ByteWidth = sizeof(tFinalPassCB);

	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, NULL, &m_pFinalPassCB)))
	{
		assert(NULL);
	}


	//=============
	// Bloom
	//=============
	
	// 다운 스케일된 HDR Texture
	D3D11_TEXTURE2D_DESC dtd = {
		m_iWidth / 4, //UINT Width;
		m_iHeight / 4, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_R16G16B16A16_FLOAT, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
	};

	if (FAILED(DEVICE->CreateTexture2D(&dtd, NULL, &m_pDownScaleRT)))
	{
		assert(NULL);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvd;
	ZeroMemory(&dsrvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	dsrvd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	dsrvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dsrvd.Texture2D.MipLevels = 1;

	if (FAILED(DEVICE->CreateShaderResourceView(m_pDownScaleRT, &dsrvd, &m_pDownScaleSRV)))
	{
		assert(NULL);
	}

	// Create the UAVs
	D3D11_UNORDERED_ACCESS_VIEW_DESC DescUAV;
	ZeroMemory(&DescUAV, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	DescUAV.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DescUAV.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	DescUAV.Buffer.FirstElement = 0;
	DescUAV.Buffer.NumElements = m_iWidth * m_iHeight / 16;

	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pDownScaleRT, &DescUAV, &m_pDownScaleUAV)))
	{
		assert(NULL);
	}

	// 필터링되기 전의 블룸 값을 임시로 저장
	if (FAILED(DEVICE->CreateTexture2D(&dtd, NULL, &m_pTempRT[0])))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateShaderResourceView(m_pTempRT[0], &dsrvd, &m_pTempSRV[0])))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pTempRT[0], &DescUAV, &m_pTempUAV[0])))
	{
		assert(NULL);
	}

	if (FAILED(DEVICE->CreateTexture2D(&dtd, NULL, &m_pTempRT[1])))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateShaderResourceView(m_pTempRT[1], &dsrvd, &m_pTempSRV[1])))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pTempRT[1], &DescUAV, &m_pTempUAV[1])))
	{
		assert(NULL);
	}

	// 최종 블룸 값

	if (FAILED(DEVICE->CreateTexture2D(&dtd, NULL, &m_pBloomRT)))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateShaderResourceView(m_pBloomRT, &dsrvd, &m_pBloomSRV)))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateUnorderedAccessView(m_pBloomRT, &DescUAV, &m_pBloomUAV)))
	{
		assert(NULL);
	}

	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory(&samDesc, sizeof(samDesc));
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = samDesc.AddressV = samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.MaxAnisotropy = 1;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	if (FAILED(DEVICE->CreateSamplerState(&samDesc, &m_pStateLinear)))
	{
		assert(NULL);
	}


	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	if (FAILED(DEVICE->CreateSamplerState(&samDesc, &m_pStatePoint)))
	{
		assert(NULL);
	}

	SetDefault();
}

void CHDRFilter::Render()
{
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(TARGET_POST);

	m_fAdaptation = 0.999f;

	if (m_fRenderTime >= m_fAdaptation)
	{
		m_fRenderTime = 0.f;
	}
	else
	{
		m_fRenderTime += CTimeMgr::GetInst()->DeltaTime();
	}

	//=============
	// Dowm Scale
	//=============
	// Constants
	D3D11_MAPPED_SUBRESOURCE MappedResource = {};
	CONTEXT->Map(m_pDownScaleCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	tDownScaleCB* pDownScale = (tDownScaleCB*)MappedResource.pData;
	pDownScale->iWidth = m_iWidth / 4;
	pDownScale->iHeight = m_iHeight / 4;
	pDownScale->iTotalPixels = pDownScale->iWidth * pDownScale->iHeight;
	pDownScale->iGroupSize = m_iDownScaleGroups;
	pDownScale->fAdaptation = min(m_fRenderTime / m_fAdaptation, 0.999f);
	pDownScale->fBloomThreshold = m_fBloomThreshold;
	CONTEXT->Unmap(m_pDownScaleCB, 0);
	ID3D11Buffer* arrConstBuffers[1] = { m_pDownScaleCB };
	CONTEXT->CSSetConstantBuffers(0, 1, arrConstBuffers);

	// Output
	ID3D11UnorderedAccessView* arrUAVs[2] = { m_pDownScale1DUAV, m_pDownScaleUAV };
	CONTEXT->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrViews[2] = { pTex->GetShaerResourceView(), NULL };
	CONTEXT->CSSetShaderResources(0, 1, arrViews);

	// Shader
	CShaderMgr::GetInst()->FindShader(L"Shader_HDR_Pass1")->UpdateData();

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	CONTEXT->Dispatch(m_iDownScaleGroups, 1, 1);

	// Second pass - reduce to a single pixel

	// Outoput
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	arrUAVs[0] = m_pAvgLumUAV;
	CONTEXT->CSSetUnorderedAccessViews(0, 2, arrUAVs, NULL);

	// Input
	arrViews[0] = m_pDownScale1DSRV;
	arrViews[1] = m_pPrevAvgLumSRV;
	CONTEXT->CSSetShaderResources(0, 2, arrViews);

	// Shader
	CShaderMgr::GetInst()->FindShader(L"Shader_HDR_Pass2")->UpdateData();

	// Excute with a single group - this group has enough threads to process all the pixels
	CONTEXT->Dispatch(1, 1, 1);

	// Cleanup
	CONTEXT->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrViews, sizeof(arrViews));
	CONTEXT->CSSetShaderResources(0, 2, arrViews);
	ZeroMemory(arrUAVs, sizeof(arrUAVs));
	CONTEXT->CSSetUnorderedAccessViews(0, 2, arrUAVs, (UINT*)(&arrUAVs));


	//=============
	// Bloom
	//=============

	// Input
	ID3D11ShaderResourceView* arrBloomViews[2] = { m_pDownScaleSRV, m_pAvgLumSRV };
	CONTEXT->CSSetShaderResources(0, 2, arrBloomViews);

	// Output
	ID3D11UnorderedAccessView* arrBloomUAVs[1] = { m_pTempUAV[0] };
	CONTEXT->CSSetUnorderedAccessViews(0, 1, arrBloomUAVs, NULL);

	// Shader
	CShaderMgr::GetInst()->FindShader(L"Shader_Bloom")->UpdateData();

	// Execute the downscales first pass with enough groups to cover the entire full res HDR buffer
	CONTEXT->Dispatch(m_iDownScaleGroups, 1, 1);

	// Cleanup
	CONTEXT->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrBloomViews, sizeof(arrBloomViews));
	CONTEXT->CSSetShaderResources(0, 2, arrBloomViews);
	ZeroMemory(arrBloomUAVs, sizeof(arrBloomUAVs));
	CONTEXT->CSSetUnorderedAccessViews(0, 1, arrBloomUAVs, NULL);


	//=============
	// Blur
	//=============

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Second pass - horizontal gaussian filter

	// Output
	ID3D11UnorderedAccessView* arrBlurUAVs[1] = { m_pTempUAV[1] };
	CONTEXT->CSSetUnorderedAccessViews(0, 1, arrBlurUAVs, NULL);

	// Input
	ID3D11ShaderResourceView* arrBlurViews[1] = { m_pTempSRV[0] };
	CONTEXT->CSSetShaderResources(0, 1, arrBlurViews);

	// Shader
	CShaderMgr::GetInst()->FindShader(L"Shader_Blur_Horizontal")->UpdateData();

	// Execute the horizontal filter
	CONTEXT->Dispatch((UINT)ceil((m_iWidth / 4.0f) / (128.0f - 12.0f)), (UINT)ceil(m_iHeight / 4.0f), 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// First pass - vertical gaussian filter

	// Output
	arrBlurUAVs[0] = m_pBloomUAV;
	CONTEXT->CSSetUnorderedAccessViews(0, 1, arrBlurUAVs, NULL);

	// Input
	arrBlurViews[0] = m_pTempSRV[1];
	CONTEXT->CSSetShaderResources(0, 1, arrBlurViews);

	// Shader
	CShaderMgr::GetInst()->FindShader(L"Shader_Blur_Vertica")->UpdateData();

	// Execute the vertical filter
	CONTEXT->Dispatch((UINT)ceil(m_iWidth / 4.0f), (UINT)ceil((m_iHeight / 4.0f) / (128.0f - 12.0f)), 1);

	// Cleanup
	CONTEXT->CSSetShader(NULL, NULL, 0);
	ZeroMemory(arrBlurViews, sizeof(arrBlurViews));
	CONTEXT->CSSetShaderResources(0, 1, arrBlurViews);
	ZeroMemory(arrBlurUAVs, sizeof(arrBlurUAVs));
	CONTEXT->CSSetUnorderedAccessViews(0, 1, arrBlurUAVs, NULL);

	ZeroMemory(&arrConstBuffers, sizeof(arrConstBuffers));
	CONTEXT->CSSetConstantBuffers(0, 1, arrConstBuffers);

	//=============
	// Final Pass
	//=============

	ID3D11ShaderResourceView* arrViewsFinal[3] = { pTex->GetShaerResourceView(), m_pAvgLumSRV,  m_pBloomSRV };
	CONTEXT->PSSetShaderResources(0, 3, arrViewsFinal);

	// Constants
	MappedResource = {};
	CONTEXT->Map(m_pFinalPassCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	tFinalPassCB* pFinalPass = (tFinalPassCB*)MappedResource.pData;
	pFinalPass->fMiddleGrey = m_fMiddleGrey;
	pFinalPass->fLumWhiteSqr = m_fWhite;
	pFinalPass->fLumWhiteSqr *= pFinalPass->fMiddleGrey; // Scale by the middle grey value
	pFinalPass->fLumWhiteSqr *= pFinalPass->fLumWhiteSqr; // Squre
	pFinalPass->fBloomScale = m_fBloomScale;
	CONTEXT->Unmap(m_pFinalPassCB, 0);
	ID3D11Buffer* arrConstBuffersFinal[1] = { m_pFinalPassCB };
	CONTEXT->PSSetConstantBuffers(2, 1, arrConstBuffersFinal);

	ID3D11SamplerState* arrSamplers[2] = { m_pStatePoint, m_pStateLinear };
	CONTEXT->PSSetSamplers(0, 2, arrSamplers);
	
	m_Trans.UpdateData();
	m_pMtrl->GetShader()->UpdateData();

	m_pMesh->SetLayout(m_pMtrl->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();
	m_pMesh->Render();

	// Cleanup
	ZeroMemory(arrViewsFinal, sizeof(arrViewsFinal));
	CONTEXT->PSSetShaderResources(0, 3, arrViewsFinal);
	ZeroMemory(arrConstBuffers, sizeof(arrConstBuffers));
	CONTEXT->PSSetConstantBuffers(0, 1, arrConstBuffers);
	CONTEXT->VSSetShader(NULL, NULL, 0);
	CONTEXT->PSSetShader(NULL, NULL, 0);

	// Swap the previous frame average luminance
	ID3D11Buffer* pTempBuffer = m_pAvgLumBuffer;
	ID3D11UnorderedAccessView* pTempUAV = m_pAvgLumUAV;
	ID3D11ShaderResourceView* p_TempSRV = m_pAvgLumSRV;
	m_pAvgLumBuffer = m_pPrevAvgLumBuffer;
	m_pAvgLumUAV = m_pPrevAvgLumUAV;
	m_pAvgLumSRV = m_pPrevAvgLumSRV;
	m_pPrevAvgLumBuffer = pTempBuffer;
	m_pPrevAvgLumUAV = pTempUAV;
	m_pPrevAvgLumSRV = p_TempSRV;

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_F5, KEY_STATE::STATE_TAB))
	{
		SetDefault();
	}
}

void CHDRFilter::SetDefault()
{
	m_fMiddleGrey = 8.f;
	m_fWhite = 5.f;

	m_fBloomThreshold = 1.3f;
	m_fBloomScale = 1.8f;
}
