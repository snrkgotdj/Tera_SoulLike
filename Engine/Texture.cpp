#include "Texture.h"

#include "PathMgr.h"
#include "ResMgr.h"
#include "Device.h"

CTexture::CTexture()
	: m_pImage(NULL)
	, m_pTex2D(NULL)
	, m_pSRV(NULL)
	, m_pRTV(NULL)
	, m_pDSV(NULL)
	, m_tDesc{}
{
}

CTexture::~CTexture()
{
	SAFE_DELETE(m_pImage);

	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pRTV);
	SAFE_RELEASE(m_pDSV);

	SAFE_RELEASE(m_pTex2D);
}

CTexture * CTexture::Create(const wstring & _strFileName)
{
	wstring strExt = CPathMgr::GetExt(_strFileName.c_str());
	HRESULT hr = 0;
	ScratchImage* pImage = new ScratchImage;

	bool bFail = false;
	if (strExt == L".dds" || strExt == L".DDS")
	{
		hr = LoadFromDDSFile(_strFileName.c_str(), DDS_FLAGS_NONE, NULL, *pImage);
		if (FAILED(hr))
		{
			bFail = true;
		}
	}
	else if (strExt == L".tga" || strExt == L".TGA")
	{
		hr = LoadFromTGAFile(_strFileName.c_str(), NULL, *pImage);
		if (FAILED(hr))
		{
			bFail = true;
		}
	}
	else
	{
		hr = LoadFromWICFile(_strFileName.c_str(), WIC_FLAGS_NONE, NULL, *pImage);
		if (FAILED(hr))
		{
			bFail = true;
		}
	}

	if (bFail)
	{
		SAFE_DELETE(pImage);
		return NULL;
	}

	//if (pImage->GetMetadata().format != DXGI_FORMAT_R8G8B8A8_UNORM && pImage->GetMetadata().format != DXGI_FORMAT_BC1_UNORM)
	//{
	//	ScratchImage* pSrcImage = pImage;
	//	pImage = new ScratchImage;

	//	HRESULT hr = Convert(pSrcImage->GetImages(), pSrcImage->GetImageCount(),
	//		pSrcImage->GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM, TEX_FILTER_DEFAULT
	//		, TEX_THRESHOLD_DEFAULT,
	//		*pImage);

	//	SAFE_RELEASE(pSrcImage);
	//	SAFE_DELETE(pSrcImage);

	//	if (FAILED(hr))
	//		assert(NULL);
	//}

	ID3D11ShaderResourceView* pView = NULL;

	if (FAILED(CreateShaderResourceView(DEVICE
		, pImage->GetImages(), pImage->GetImageCount(),
		pImage->GetMetadata(), &pView)))
	{
		SAFE_DELETE(pImage);
		return NULL;
	}

	CTexture* pTexture = new CTexture;
	pTexture->m_pImage = pImage;
	pTexture->m_pSRV = pView;
	pView->GetResource((ID3D11Resource**)&pTexture->m_pTex2D); // RefCount 증가
	pTexture->m_pTex2D->GetDesc(&pTexture->m_tDesc);

	return pTexture;
}

CTexture * CTexture::Create(UINT _iWidth, UINT _iHeight, UINT _iBindFlag
	, DXGI_FORMAT _eFormat, D3D11_USAGE _eUsage)
{
	D3D11_TEXTURE2D_DESC tDesc = {};

	tDesc.Width = _iWidth;
	tDesc.Height = _iHeight;
	tDesc.ArraySize = 1;
	tDesc.BindFlags = _iBindFlag;

	if (_eUsage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
	{
		tDesc.Usage = _eUsage;
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		tDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		tDesc.CPUAccessFlags = 0;
	}

	tDesc.Format = _eFormat;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.MipLevels = 1;

	ID3D11Texture2D* pTex2d = NULL;

	HRESULT hr = DEVICE->CreateTexture2D(&tDesc, NULL, &pTex2d);

	if (FAILED(hr))
		assert(NULL);

	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11RenderTargetView* pRTV = NULL;
	ID3D11DepthStencilView* pDSV = NULL;
	ScratchImage* pImage = NULL;

	if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		hr = DEVICE->CreateDepthStencilView(pTex2d, 0, &pDSV);
		if (FAILED(hr))
			assert(NULL);
	}
	else
	{
		if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
		{
			hr = DEVICE->CreateRenderTargetView(pTex2d, 0, &pRTV);
			if (FAILED(hr))
				assert(NULL);
		}
		if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

			tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			tSRVDesc.Texture2D.MipLevels = 1;
			tSRVDesc.Texture2D.MostDetailedMip = 0;

			hr = DEVICE->CreateShaderResourceView(pTex2d, &tSRVDesc, &pSRV);
			if (FAILED(hr))
				assert(NULL);
		}

		pImage = new ScratchImage;
		hr = CaptureTexture(DEVICE, CONTEXT, pTex2d, *pImage);
		if (FAILED(hr))
			assert(NULL);
	}

	CTexture* pTexture = new CTexture;
	pTexture->m_pRTV = pRTV;
	pTexture->m_pSRV = pSRV;
	pTexture->m_pDSV = pDSV;

	pTexture->m_pTex2D = pTex2d;
	pTexture->m_tDesc = tDesc;

	pTexture->m_pImage = pImage;

	return pTexture;
}

CTexture * CTexture::Create(ID3D11Texture2D * _pTex2D, UINT _iBindFlag, DXGI_FORMAT _eFormat)
{
	HRESULT hr = 0;
	ID3D11ShaderResourceView* pSRV = NULL;
	ID3D11RenderTargetView* pRTV = NULL;
	ID3D11DepthStencilView* pDSV = NULL;

	if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET)
	{
		hr = DEVICE->CreateRenderTargetView(_pTex2D, 0, &pRTV);
		if (FAILED(hr))
			assert(NULL);
	}
	if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

		tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
		tSRVDesc.Texture2D.MipLevels = 1;
		tSRVDesc.Texture2D.MostDetailedMip = 0;

		hr = DEVICE->CreateShaderResourceView(_pTex2D, &tSRVDesc, &pSRV);
		if (FAILED(hr))
			assert(NULL);
	}
	if (_iBindFlag & D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL)
	{
		assert(NULL);
	}

	ScratchImage* pImage = new ScratchImage;
	hr = CaptureTexture(DEVICE, CONTEXT, _pTex2D, *pImage);
	if (FAILED(hr))
		assert(NULL);

	CTexture* pTexture = new CTexture;
	pTexture->m_pRTV = pRTV;
	pTexture->m_pSRV = pSRV;
	pTexture->m_pDSV = pDSV;

	pTexture->m_pTex2D = _pTex2D;
	_pTex2D->GetDesc(&pTexture->m_tDesc);

	pTexture->m_pImage = pImage;

	return pTexture;
}

CTexture * CTexture::Create(vector<CTexture*>& _vecTex, D3D11_USAGE _eUsage)
{
	// 원본 텍스쳐의 임시데이터를 만든다(읽을 수 있는 버전으로)
	D3D11_TEXTURE2D_DESC tDesc = {};

	vector<ID3D11Texture2D*> vecTempTex;
	for (UINT i = 0; i < _vecTex.size(); ++i)
	{
		_vecTex[i]->m_pTex2D->GetDesc(&tDesc);
		tDesc.BindFlags = 0;
		tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tDesc.Usage = D3D11_USAGE_STAGING;

		D3D11_SUBRESOURCE_DATA tSub = {};
		tSub.pSysMem = _vecTex[i]->GetPixel();
		tSub.SysMemPitch = _vecTex[i]->m_pImage->GetImages()->rowPitch;
		tSub.SysMemSlicePitch = _vecTex[i]->m_pImage->GetImages()->slicePitch;

		ID3D11Texture2D* pTempTex = NULL;
		DEVICE->CreateTexture2D(&tDesc, &tSub, &pTempTex);
		vecTempTex.push_back(pTempTex);
	}

	_vecTex[0]->m_pTex2D->GetDesc(&tDesc);
	D3D11_TEXTURE2D_DESC tArrDesc = {};
	tArrDesc.Width = tDesc.Width;
	tArrDesc.Height = tDesc.Height;
	tArrDesc.MipLevels = tDesc.MipLevels;
	tArrDesc.Format = tDesc.Format;
	tArrDesc.ArraySize = vecTempTex.size();
	tArrDesc.Usage = _eUsage;
	tArrDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tArrDesc.SampleDesc.Count = 1;
	tArrDesc.SampleDesc.Quality = 0;

	ID3D11Texture2D* pMultiTex = NULL;

	// 멀티텍스쳐 만들기
	HRESULT hr = DEVICE->CreateTexture2D(&tArrDesc, NULL, &pMultiTex);
	if (FAILED(hr))
		assert(NULL);

	for (UINT i = 0; i < vecTempTex.size(); ++i)
	{
		D3D11_MAPPED_SUBRESOURCE tSub = {};
		CONTEXT->Map(vecTempTex[i], 0, D3D11_MAP_READ, 0, &tSub);

		CONTEXT->UpdateSubresource(pMultiTex
			, D3D11CalcSubresource(0, i, tArrDesc.MipLevels)
			, NULL
			, tSub.pData
			, tSub.RowPitch
			, tSub.DepthPitch);

		CONTEXT->Unmap(vecTempTex[i], 0);
	}

	// 만들어진 Multi Texture를 이용해서 ShaderResourceView 만들기
	D3D11_SHADER_RESOURCE_VIEW_DESC tViewDesc = {};
	tViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tViewDesc.Texture2DArray.ArraySize = vecTempTex.size();
	tViewDesc.Texture2DArray.FirstArraySlice = 0;
	tViewDesc.Texture2DArray.MipLevels = tDesc.MipLevels;
	tViewDesc.Texture2DArray.MostDetailedMip = 0;

	ID3D11ShaderResourceView* pSRV = NULL;

	if (FAILED(DEVICE->CreateShaderResourceView(pMultiTex, &tViewDesc, &pSRV)))
		assert(NULL);

	ScratchImage* pImage = new ScratchImage;
	if (FAILED(CaptureTexture(DEVICE, CONTEXT, pMultiTex, (*pImage))))
		assert(NULL);

	CTexture* pTex = new CTexture;
	pTex->m_pImage = pImage;
	pTex->m_pSRV = pSRV;
	pTex->m_pTex2D = pMultiTex;
	pTex->m_tDesc = tArrDesc;

	// 임시 텍스쳐들 해제
	for (UINT i = 0; i < vecTempTex.size(); ++i)
	{
		SAFE_RELEASE(vecTempTex[i]);
	}

	return pTex;
}

bool CTexture::IsDynamic()
{
	if (m_tDesc.Usage == D3D11_USAGE_DYNAMIC)
		return true;
	return false;
}

void CTexture::ChangeDynamic()
{
	m_tDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	ID3D11Texture2D* pTex2D = NULL;
	D3D11_SUBRESOURCE_DATA tSub = {};	

	tSub.pSysMem = m_pImage->GetPixels();
	tSub.SysMemPitch = m_pImage->GetImages()->rowPitch;
	tSub.SysMemSlicePitch = m_pImage->GetImages()->slicePitch;

	DEVICE->CreateTexture2D(&m_tDesc, &tSub, &pTex2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC tResDesc = {};
	tResDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tResDesc.Texture2D.MipLevels = m_tDesc.MipLevels;

	ID3D11ShaderResourceView * pView = NULL;
	DEVICE->CreateShaderResourceView(pTex2D, &tResDesc, &pView);

	SAFE_RELEASE(m_pTex2D);
	SAFE_RELEASE(m_pSRV);

	m_pTex2D = pTex2D;
	m_pSRV = pView;
}

void CTexture::SaveTexture(const wstring & _strPath)
{
	wstring strPath = CPathMgr::GetResPath();
	strPath += _strPath;

	SaveToWICFile(*(m_pImage->GetImages())
		, WIC_FLAGS_NONE
		, GetWICCodec(static_cast<WICCodecs>(WIC_CODEC_PNG))
		, strPath.c_str());

}

void CTexture::Reset()
{
	/*D3D11_MAPPED_SUBRESOURCE tData = {};

	CONTEXT->Map(m_pTex2D, 0, D3D11_MAP_WRITE_DISCARD, 0 , &tData);
	
	UINT iSize = m_pImage->GetPixelsSize();
	memcpy(tData.pData, m_pImage->GetPixels(), m_pImage->GetPixelsSize());	

	CONTEXT->Unmap(m_pTex2D, 0);*/
	
	ID3D11ShaderResourceView* pView = NULL;

	CreateShaderResourceView(DEVICE, m_pImage->GetImages(), m_pImage->GetImageCount()
		, m_pImage->GetMetadata(), &pView);
	
	ID3D11Texture2D* pTex2D = NULL;
	pView->GetResource((ID3D11Resource**)&pTex2D);
	
	SAFE_RELEASE(m_pTex2D);
	SAFE_RELEASE(m_pSRV);

	m_pTex2D = pTex2D;
	m_pSRV = pView;
}

void CTexture::Resize(UINT _iWidth, UINT _iHeight)
{
	ID3D11Texture2D* pTex2D = NULL;

	m_tDesc.Width = _iWidth;
	m_tDesc.Height = _iHeight;

	DEVICE->CreateTexture2D(&m_tDesc, NULL, &pTex2D);

	D3D11_SHADER_RESOURCE_VIEW_DESC tResDesc = {};
	tResDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	tResDesc.Texture2D.MipLevels = m_tDesc.MipLevels;

	ID3D11ShaderResourceView * pView = NULL;
	DEVICE->CreateShaderResourceView(pTex2D, &tResDesc, &pView);

	SAFE_RELEASE(m_pSRV);
	SAFE_RELEASE(m_pTex2D);

	m_pSRV = pView;
	m_pTex2D = pTex2D;
}

void CTexture::UpdateData(UINT _iRegister, UINT _iTiming)
{
	if (_iTiming & (UINT)SHADER_TYPE::ST_VERTEX)
		CONTEXT->VSSetShaderResources(_iRegister, 1, &m_pSRV);
	if (_iTiming & (UINT)SHADER_TYPE::ST_HULL)
		CONTEXT->HSSetShaderResources(_iRegister, 1, &m_pSRV);
	if (_iTiming & (UINT)SHADER_TYPE::ST_DOMAIN)
		CONTEXT->DSSetShaderResources(_iRegister, 1, &m_pSRV);
	if (_iTiming & (UINT)SHADER_TYPE::ST_GEOMETRY)
		CONTEXT->GSSetShaderResources(_iRegister, 1, &m_pSRV);
	if (_iTiming & (UINT)SHADER_TYPE::ST_COMPUTE)
		CONTEXT->CSSetShaderResources(_iRegister, 1, &m_pSRV);
	if (_iTiming & (UINT)SHADER_TYPE::ST_PIXEL)
		CONTEXT->PSSetShaderResources(_iRegister, 1, &m_pSRV);
}

void CTexture::ClearData(UINT _iRegister, UINT _iTiming)
{
	ID3D11ShaderResourceView* pView = NULL;

	if (_iTiming & (UINT)SHADER_TYPE::ST_VERTEX)
		CONTEXT->VSSetShaderResources(_iRegister, 1, &pView);
	if (_iTiming & (UINT)SHADER_TYPE::ST_HULL)
		CONTEXT->HSSetShaderResources(_iRegister, 1, &pView);
	if (_iTiming & (UINT)SHADER_TYPE::ST_DOMAIN)
		CONTEXT->DSSetShaderResources(_iRegister, 1, &pView);
	if (_iTiming & (UINT)SHADER_TYPE::ST_GEOMETRY)
		CONTEXT->GSSetShaderResources(_iRegister, 1, &pView);
	if (_iTiming & (UINT)SHADER_TYPE::ST_COMPUTE)
		CONTEXT->CSSetShaderResources(_iRegister, 1, &pView);
	if (_iTiming & (UINT)SHADER_TYPE::ST_PIXEL)
		CONTEXT->PSSetShaderResources(_iRegister, 1, &pView);
}

void CTexture::SetData(void * _pSrc, size_t _size)
{
	D3D11_MAPPED_SUBRESOURCE tData = {};

	CONTEXT->Map(m_pTex2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &tData);

	memcpy(tData.pData, _pSrc, _size);

	CONTEXT->Unmap(m_pTex2D, 0);
}
