#include "stdafx.h"
#include "ShaderParam.h"

CShaderParam::CShaderParam()
{
}

CShaderParam::CShaderParam(UINT nIDTemplate, CWnd * pParent)
	: CDialogEx(nIDTemplate, pParent)
	, m_tParamInfo {}
	, m_pMtrl(NULL)
{
}

CShaderParam::~CShaderParam()
{
}

void CShaderParam::Init(CMaterial* _pMtrl, const tShaderParam& _tParam)
{
	m_pMtrl = _pMtrl;
	m_tParamInfo = _tParam;
}