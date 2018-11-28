#pragma once
#include "afxdialogex.h"

class CMaterial;

class CShaderParam :
	public CDialogEx
{
protected:
	tShaderParam	m_tParamInfo;
	CMaterial*		m_pMtrl;

public:
	virtual void Init(CMaterial* _pMtrl, const tShaderParam& _tParam);
	virtual void UpdateParam() = 0;

public:
	CShaderParam();
	CShaderParam(UINT nIDTemplate, CWnd *pParent = NULL);
	virtual ~CShaderParam();
};

