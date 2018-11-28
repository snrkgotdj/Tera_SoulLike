#include "Shader.h"

#include "Device.h"
#include "Sampler.h"
#include "BlendState.h"
#include "DepthStencilState.h"

CShader::CShader()
	: m_pVSBlob(NULL)
	, m_pVSInstBlob(NULL)
	, m_pHSBlob(NULL)
	, m_pDSBlob(NULL)
	, m_pCSBlob(NULL)
	, m_pGSBlob(NULL)
	, m_pPSBlob(NULL)
	, m_pErrBlob(NULL)
	, m_pVS(NULL)
	, m_pVSInst(NULL)
	, m_pHS(NULL)
	, m_pDS(NULL)
	, m_pGS(NULL)
	, m_pCS(NULL)
	, m_pPS(NULL)
	, m_pBlendState(NULL)
	, m_pDepthStencilState(NULL)
	, m_iStencilRef(0)
	, m_bDeferred(false)
{
}

CShader::~CShader()
{
	SAFE_RELEASE(m_pVSBlob);
	SAFE_RELEASE(m_pVSInstBlob);
	SAFE_RELEASE(m_pHSBlob);
	SAFE_RELEASE(m_pDSBlob);
	SAFE_RELEASE(m_pGSBlob);
	SAFE_RELEASE(m_pCSBlob);
	SAFE_RELEASE(m_pPSBlob);

	SAFE_RELEASE(m_pErrBlob);

	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pVSInst);
	SAFE_RELEASE(m_pHS);
	SAFE_RELEASE(m_pDS);
	SAFE_RELEASE(m_pGS);
	SAFE_RELEASE(m_pCS);
	SAFE_RELEASE(m_pPS);
}

int CShader::CreateVertexShader(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "vs_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pVSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_pVSBlob->GetBufferPointer(), m_pVSBlob->GetBufferSize()
		, NULL, &m_pVS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreateVertexShader_Instancing(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "vs_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pVSInstBlob, &m_pErrBlob)))
	{
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateVertexShader(m_pVSInstBlob->GetBufferPointer()
		, m_pVSInstBlob->GetBufferSize()
		, NULL, &m_pVSInst)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreateHullShader(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "hs_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pHSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateHullShader(m_pHSBlob->GetBufferPointer(), m_pHSBlob->GetBufferSize()
		, NULL, &m_pHS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreateDomainShader(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "ds_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pDSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateDomainShader(m_pDSBlob->GetBufferPointer(), m_pDSBlob->GetBufferSize()
		, NULL, &m_pDS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreateGeometryShader(const wstring & _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "gs_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pGSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateGeometryShader(m_pGSBlob->GetBufferPointer(), m_pGSBlob->GetBufferSize()
		, NULL, &m_pGS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreateComputeShader(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "cs_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pCSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreateComputeShader(m_pCSBlob->GetBufferPointer(), m_pCSBlob->GetBufferSize()
		, NULL, &m_pCS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

int CShader::CreatePixelShader(wstring _strFilePath, char * _pFuncName, int _iHigh, int _iRow)
{
	UINT iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D10_SHADER_DEBUG;
#endif

	char strVersion[255] = "";
	sprintf_s(strVersion, "ps_%d_%d", _iHigh, _iRow);

	if (FAILED(D3DCompileFromFile(_strFilePath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, _pFuncName, strVersion, iFlag, 0, &m_pPSBlob, &m_pErrBlob)))
	{
		wstring strErr = GetErr();
		return RET_FAILED;
	}

	if (FAILED(DEVICE->CreatePixelShader(m_pPSBlob->GetBufferPointer(), m_pPSBlob->GetBufferSize()
		, NULL, &m_pPS)))
	{
		return RET_FAILED;
	}

	return RET_SUCCESS;
}

wstring CShader::GetErr()
{
	if (NULL != m_pErrBlob)
	{
		string strErr = (char*)m_pErrBlob->GetBufferPointer();
		return wstring(strErr.begin(), strErr.end());
	}
	return wstring();
}


void CShader::UpdateData()
{
	CONTEXT->VSSetShader(m_pVS, NULL, NULL);
	CONTEXT->HSSetShader(m_pHS, NULL, NULL);
	CONTEXT->DSSetShader(m_pDS, NULL, NULL);
	CONTEXT->GSSetShader(m_pGS, NULL, NULL);
	CONTEXT->CSSetShader(m_pCS, NULL, NULL);
	CONTEXT->PSSetShader(m_pPS, NULL, NULL);

	for (UINT i = 0; i < m_vecSampler.size(); ++i)
	{
		m_vecSampler[i].pSampler->UpdateData(m_vecSampler[i].iTiming);
	}

	if (NULL != m_pBlendState)
		m_pBlendState->UpdateData();
	else
		CONTEXT->OMSetBlendState(NULL, NULL, 0xffffffff);

	if (NULL != m_pDepthStencilState)
		m_pDepthStencilState->UpdateData(m_iStencilRef);
	else
		CONTEXT->OMSetDepthStencilState(NULL, m_iStencilRef);
}

void CShader::UpdateDataInstancing()
{
	CONTEXT->VSSetShader(m_pVSInst, NULL, NULL);
	CONTEXT->HSSetShader(m_pHS, NULL, NULL);
	CONTEXT->DSSetShader(m_pDS, NULL, NULL);
	CONTEXT->GSSetShader(m_pGS, NULL, NULL);
	CONTEXT->CSSetShader(m_pCS, NULL, NULL);
	CONTEXT->PSSetShader(m_pPS, NULL, NULL);

	for (UINT i = 0; i < m_vecSampler.size(); ++i)
	{
		m_vecSampler[i].pSampler->UpdateData(m_vecSampler[i].iTiming);
	}

	if (NULL != m_pBlendState)
		m_pBlendState->UpdateData();
	else
		CONTEXT->OMSetBlendState(NULL, NULL, 0xffffffff);

	if (NULL != m_pDepthStencilState)
		m_pDepthStencilState->UpdateData();
	else
		CONTEXT->OMSetDepthStencilState(NULL, NULL);
}

void CShader::AddSampler(CSampler * _pSampler, UINT _iTiming)
{
	m_vecSampler.push_back(SAMPLER_INFO{ _pSampler, _iTiming });
}

void CShader::AddShaderParam(SHADER_PARAM _eParam, UINT _iTiming, const wstring& _strName)
{
	tShaderParam param = {};
	param.eShaderParam = _eParam;
	param.iRegister = g_SPRegister[(UINT)_eParam];
	param.iTiming = _iTiming;
	param.strParamName = _strName;

	m_vecShaderParam.push_back(param);
}
