#pragma once

#include "global.h"

class CSampler;
class CBlendState;
class CDepthStencilState;

struct SAMPLER_INFO
{
	CSampler*	pSampler;
	UINT		iTiming;
};

class CShader
{
private:
	ID3DBlob*				m_pVSBlob;
	ID3DBlob*				m_pVSInstBlob;
	ID3DBlob*				m_pHSBlob;
	ID3DBlob*				m_pDSBlob;
	ID3DBlob*				m_pGSBlob;
	ID3DBlob*				m_pCSBlob;
	ID3DBlob*				m_pPSBlob;

	ID3DBlob*				m_pErrBlob;

	ID3D11VertexShader *	m_pVS;
	ID3D11VertexShader *	m_pVSInst;
	ID3D11HullShader*		m_pHS;
	ID3D11DomainShader*		m_pDS;
	ID3D11GeometryShader*   m_pGS;
	ID3D11ComputeShader*	m_pCS;
	ID3D11PixelShader *		m_pPS;

	vector<SAMPLER_INFO>	m_vecSampler;
	vector<tShaderParam>    m_vecShaderParam;
	CBlendState*			m_pBlendState;
	CDepthStencilState*     m_pDepthStencilState;
	UINT					m_iStencilRef;

	wstring					m_strKey;
	bool					m_bDeferred;

public:
	wstring GetErr();

	ID3DBlob* GetVSBlob() { return m_pVSBlob; }
	ID3DBlob* GetVSInstBlob() { return m_pVSInstBlob; }
	ID3DBlob* GetPSBlob() { return m_pPSBlob; }

	void SetBlendState(CBlendState* _pState) { m_pBlendState = _pState; }
	void SetDepthStencilState(CDepthStencilState* _pState, UINT _iStencilRef = 0) { m_pDepthStencilState = _pState; m_iStencilRef = _iStencilRef; }
	void SetStencilRef(UINT _iStencilRef) { m_iStencilRef = _iStencilRef; }
	const wstring& GetKey() { return m_strKey; }
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
	void SetDeferred() { m_bDeferred = true; }
	bool IsDeferred() { return m_bDeferred; }

public:
	int CreateVertexShader(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreateVertexShader_Instancing(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreateHullShader(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreateDomainShader(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreateGeometryShader(const wstring& _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreateComputeShader(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	int CreatePixelShader(wstring _strFilePath, char* _pFuncName, int _iHigh, int _iRow);
	void UpdateData();
	void UpdateDataInstancing();

public:
	void AddSampler(CSampler* _pSampler, UINT _iTiming);
	void AddShaderParam(SHADER_PARAM _eParam, UINT _iTiming, const wstring& _strName = L"");
	vector<tShaderParam>& GetShaderParam() { return m_vecShaderParam; }

public:
	CShader();
	~CShader();
};

