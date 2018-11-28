#pragma once
#include "Res.h"

struct tShaderParam_EX : public tShaderParam
{
	void* pData;
};

class CShader;

class CMaterial :
	public CRes
{
private:
	CShader*				m_pShader;
	vector<tShaderParam_EX> m_vecParam;

public:
	void SetShader(CShader* _pShader);
	CShader* GetShader() { return m_pShader; }

private:
	void ClearShaderParam();
	void AdjustShaderParam();
	void UpdateParam();
	vector<tShaderParam_EX> GetVecPram() { return m_vecParam; }

public:
	void SetData(const SHADER_PARAM& _eParam, void* _pData);
	void* GetData(const SHADER_PARAM& _eParam);
	void UpdateData();
	void UpdateDataInstancing();
	
public:
	virtual CMaterial* Clone() { return new CMaterial(*this); }
	virtual void Save(FILE* _pFile);	
	static CRes* Load(FILE* _pFile);
	void LoadShaderParam(FILE* _pFile);

public:
	CMaterial();
	CMaterial(const CMaterial& _other);
	virtual ~CMaterial();

	friend class CMaterialDataDlg;
};

