#pragma once
#include "CamFilter.h"

typedef struct
{
	UINT	iWidth;
	UINT	iHeight;
	UINT	iTotalPixels;
	UINT	iGroupSize;
	float	fAdaptation;
	float	fBloomThreshold;
	UINT	pad[2];
} tDownScaleCB;

typedef struct
{
	float	fMiddleGrey;
	float	fLumWhiteSqr;
	float	fBloomScale;
	UINT pad;
} tFinalPassCB;

class CHDRFilter :
	public CCamFilter
{
private:
	// 1D intermediate storage for the down scale operation
	ID3D11Buffer*				m_pDownScale1DBuffer;
	ID3D11UnorderedAccessView*	m_pDownScale1DUAV;
	ID3D11ShaderResourceView*	m_pDownScale1DSRV;

	// Average luminance
	ID3D11Buffer*				m_pAvgLumBuffer;
	ID3D11UnorderedAccessView*	m_pAvgLumUAV;
	ID3D11ShaderResourceView*	m_pAvgLumSRV;

	// Prev Average luminance
	ID3D11Buffer*				m_pPrevAvgLumBuffer;
	ID3D11UnorderedAccessView*	m_pPrevAvgLumUAV;
	ID3D11ShaderResourceView*	m_pPrevAvgLumSRV;

	// Const Buffer
	ID3D11Buffer*				m_pDownScaleCB;
	ID3D11Buffer*				m_pFinalPassCB;

	// Downscaled scene texture
	ID3D11Texture2D*			m_pDownScaleRT;
	ID3D11ShaderResourceView*	m_pDownScaleSRV;
	ID3D11UnorderedAccessView*	m_pDownScaleUAV;

	// Temporary texture
	ID3D11Texture2D*			m_pTempRT[2];
	ID3D11ShaderResourceView*	m_pTempSRV[2];
	ID3D11UnorderedAccessView*	m_pTempUAV[2];

	// Bloom texture
	ID3D11Texture2D*			m_pBloomRT;
	ID3D11ShaderResourceView*	m_pBloomSRV;
	ID3D11UnorderedAccessView*	m_pBloomUAV;

	ID3D11SamplerState*			m_pStateLinear;
	ID3D11SamplerState*			m_pStatePoint;

	UINT						m_iDownScaleGroups;
	UINT						m_iWidth;
	UINT						m_iHeight;

	float						m_fMiddleGrey;
	float						m_fWhite;

	float						m_fAdaptation;
	float						m_fRenderTime;

	float						m_fBloomThreshold;
	float						m_fBloomScale;

public:
	void Init();
	virtual void Render();
	
public:
	void SetDefault();

public:
	void SetBloomValue(float _fThreshold, float _fScale) { m_fBloomThreshold = _fThreshold;  m_fBloomScale = _fScale; }

public:
	CHDRFilter();
	virtual ~CHDRFilter();
};