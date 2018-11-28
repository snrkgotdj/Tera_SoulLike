#include "Material.h"

#include "Shader.h"
#include "Texture.h"
#include "Device.h"
#include "ShaderMgr.h"
#include "ResMgr.h"

UINT g_SPRegister[(UINT)SHADER_PARAM::SP_END]
{
	1,//SP_INT_0,
	1,//SP_INT_1,
	1,//SP_INT_2,
	1,//SP_INT_3,
	1,//SP_INT_4,
	1,//SP_INT_5,
	1,//SP_INT_6,
	1,//SP_INT_7,
	1,//SP_INT_END,
	2,//SP_FLOAT_0,
	2,//SP_FLOAT_1,
	2,//SP_FLOAT_2,
	2,//SP_FLOAT_3,
	2,//SP_FLOAT_4,
	2,//SP_FLOAT_5,
	2,//SP_FLOAT_6,
	2,//SP_FLOAT_7,
	2,//SP_FLOAT_END,
	3,//SP_VEC4_0,
	3,//SP_VEC4_1,
	3,//SP_VEC4_2,
	3,//SP_VEC4_3,
	3,//SP_VEC4_4,
	3,//SP_VEC4_5,
	3,//SP_VEC4_6,
	3,//SP_VEC4_7,
	3,//SP_VEC4_END,
	0, //SP_TEX_0,
	1, //SP_TEX_1,
	2, //SP_TEX_2,
	3, //SP_TEX_3,
	4, //SP_TEX_4,
	5, //SP_TEX_5,
	6, //SP_TEX_6,
	7, //SP_TEX_7,
	8, //SP_TEXARR_0,
	9, //SP_TEXARR_1,
	10,//SP_TEXARR_2,
	11,//SP_TEXARR_3,
	12,//SP_TEXARR_4,
	13,//SP_TEXARR_5,
	14,//SP_TEXARR_6,
	15,//SP_TEXARR_7,
	0,//SP_TEX_END,	
};

wchar_t* g_SPName[(UINT)SHADER_PARAM::SP_END]
{
	L"SP_INT_0"		,
	L"SP_INT_1"		,
	L"SP_INT_2"		,
	L"SP_INT_3"		,
	L"SP_INT_4"		,
	L"SP_INT_5"		,
	L"SP_INT_6"		,
	L"SP_INT_7"		,
	L"SP_INT_END"	,
	L"SP_FLOAT_0"	,
	L"SP_FLOAT_1"	,
	L"SP_FLOAT_2"	,
	L"SP_FLOAT_3"	,
	L"SP_FLOAT_4"	,
	L"SP_FLOAT_5"	,
	L"SP_FLOAT_6"	,
	L"SP_FLOAT_7"	,
	L"SP_FLOAT_END"	,
	L"SP_VEC4_0"	,
	L"SP_VEC4_1"	,
	L"SP_VEC4_2"	,
	L"SP_VEC4_3"	,
	L"SP_VEC4_4"	,
	L"SP_VEC4_5"	,
	L"SP_VEC4_6"	,
	L"SP_VEC4_7"	,
	L"SP_VEC4_END"	,
	L"SP_TEX_0",
	L"SP_TEX_1",
	L"SP_TEX_2",
	L"SP_TEX_3",
	L"SP_TEX_4",
	L"SP_TEX_5",
	L"SP_TEX_6",
	L"SP_TEX_7",
	L"SP_TEXARR_0",
	L"SP_TEXARR_1",
	L"SP_TEXARR_2",
	L"SP_TEXARR_3",
	L"SP_TEXARR_4",
	L"SP_TEXARR_5",
	L"SP_TEXARR_6",
	L"SP_TEXARR_7",
	L"SP_TEX_END",
};

CMaterial::CMaterial()
	: m_pShader(NULL)
{
}

CMaterial::CMaterial(const CMaterial & _other)
	: CRes(_other)
	, m_pShader(_other.m_pShader)
	, m_vecParam(_other.m_vecParam)
{
	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		switch (m_vecParam[i].eShaderParam)
		{
		case SHADER_PARAM::SP_INT_0:
		case SHADER_PARAM::SP_INT_1:
		case SHADER_PARAM::SP_INT_2:
		case SHADER_PARAM::SP_INT_3:
		case SHADER_PARAM::SP_INT_4:
		case SHADER_PARAM::SP_INT_5:
		case SHADER_PARAM::SP_INT_6:
		case SHADER_PARAM::SP_INT_7:
		{
			m_vecParam[i].pData = new int;
			memcpy(m_vecParam[i].pData, _other.m_vecParam[i].pData, sizeof(int));
		}
		break;
		case SHADER_PARAM::SP_FLOAT_0:
		case SHADER_PARAM::SP_FLOAT_1:
		case SHADER_PARAM::SP_FLOAT_2:
		case SHADER_PARAM::SP_FLOAT_3:
		case SHADER_PARAM::SP_FLOAT_4:
		case SHADER_PARAM::SP_FLOAT_5:
		case SHADER_PARAM::SP_FLOAT_6:
		case SHADER_PARAM::SP_FLOAT_7:
		{
			m_vecParam[i].pData = new float;
			memcpy(m_vecParam[i].pData, _other.m_vecParam[i].pData, sizeof(float));
		}
		break;
		case SHADER_PARAM::SP_VEC4_0:
		case SHADER_PARAM::SP_VEC4_1:
		case SHADER_PARAM::SP_VEC4_2:
		case SHADER_PARAM::SP_VEC4_3:
		case SHADER_PARAM::SP_VEC4_4:
		case SHADER_PARAM::SP_VEC4_5:
		case SHADER_PARAM::SP_VEC4_6:
		case SHADER_PARAM::SP_VEC4_7:
		{
			m_vecParam[i].pData = new Vec4;
			memcpy(m_vecParam[i].pData, _other.m_vecParam[i].pData, sizeof(Vec4));
		}
		break;
		case SHADER_PARAM::SP_TEX_0:
		case SHADER_PARAM::SP_TEX_1:
		case SHADER_PARAM::SP_TEX_2:
		case SHADER_PARAM::SP_TEX_3:
		case SHADER_PARAM::SP_TEX_4:
		case SHADER_PARAM::SP_TEX_5:
		case SHADER_PARAM::SP_TEX_6:
		case SHADER_PARAM::SP_TEX_7:
		case SHADER_PARAM::SP_TEXARR_0:
		case SHADER_PARAM::SP_TEXARR_1:
		case SHADER_PARAM::SP_TEXARR_2:
		case SHADER_PARAM::SP_TEXARR_3:
		case SHADER_PARAM::SP_TEXARR_4:
		case SHADER_PARAM::SP_TEXARR_5:
		case SHADER_PARAM::SP_TEXARR_6:
		case SHADER_PARAM::SP_TEXARR_7:
		{
			m_vecParam[i].pData = new void*;
			memcpy(m_vecParam[i].pData, _other.m_vecParam[i].pData, sizeof(void*));
		}
		break;
		}
	}
}

CMaterial::~CMaterial()
{
	ClearShaderParam();
}

void CMaterial::SetShader(CShader * _pShader)
{
	if (m_pShader == _pShader)
		return;

	m_pShader = _pShader;

	if (NULL != m_pShader)
	{
		ClearShaderParam();
		AdjustShaderParam();
	}
}

void CMaterial::ClearShaderParam()
{
	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		SAFE_DELETE(m_vecParam[i].pData);
	}

	m_vecParam.clear();
}

void CMaterial::AdjustShaderParam()
{
	vector<tShaderParam>& vecParam = m_pShader->GetShaderParam();

	for (UINT i = 0; i < vecParam.size(); ++i)
	{
		tShaderParam_EX param;
		param.eShaderParam = vecParam[i].eShaderParam;
		param.iRegister = vecParam[i].iRegister;
		param.iTiming = vecParam[i].iTiming;
		param.strParamName = vecParam[i].strParamName;

		switch (vecParam[i].eShaderParam)
		{
		case SHADER_PARAM::SP_INT_0:
		case SHADER_PARAM::SP_INT_1:
		case SHADER_PARAM::SP_INT_2:
		case SHADER_PARAM::SP_INT_3:
		case SHADER_PARAM::SP_INT_4:
		case SHADER_PARAM::SP_INT_5:
		case SHADER_PARAM::SP_INT_6:
		case SHADER_PARAM::SP_INT_7:
		{
			param.pData = new int;
			*((int*)param.pData) = 0;
		}
		break;
		case SHADER_PARAM::SP_FLOAT_0:
		case SHADER_PARAM::SP_FLOAT_1:
		case SHADER_PARAM::SP_FLOAT_2:
		case SHADER_PARAM::SP_FLOAT_3:
		case SHADER_PARAM::SP_FLOAT_4:
		case SHADER_PARAM::SP_FLOAT_5:
		case SHADER_PARAM::SP_FLOAT_6:
		case SHADER_PARAM::SP_FLOAT_7:
		{
			param.pData = new float;
			*((float*)param.pData) = 0;
		}
		break;
		case SHADER_PARAM::SP_VEC4_0:
		case SHADER_PARAM::SP_VEC4_1:
		case SHADER_PARAM::SP_VEC4_2:
		case SHADER_PARAM::SP_VEC4_3:
		case SHADER_PARAM::SP_VEC4_4:
		case SHADER_PARAM::SP_VEC4_5:
		case SHADER_PARAM::SP_VEC4_6:
		case SHADER_PARAM::SP_VEC4_7:
		{
			param.pData = new Vec4;
			*((Vec4*)param.pData) = Vec4(0.f, 0.f, 0.f, 0.f);
		}
		break;
		case SHADER_PARAM::SP_TEX_0:
		case SHADER_PARAM::SP_TEX_1:
		case SHADER_PARAM::SP_TEX_2:
		case SHADER_PARAM::SP_TEX_3:
		case SHADER_PARAM::SP_TEX_4:
		case SHADER_PARAM::SP_TEX_5:
		case SHADER_PARAM::SP_TEX_6:
		case SHADER_PARAM::SP_TEX_7:
		case SHADER_PARAM::SP_TEXARR_0:
		case SHADER_PARAM::SP_TEXARR_1:
		case SHADER_PARAM::SP_TEXARR_2:
		case SHADER_PARAM::SP_TEXARR_3:
		case SHADER_PARAM::SP_TEXARR_4:
		case SHADER_PARAM::SP_TEXARR_5:
		case SHADER_PARAM::SP_TEXARR_6:
		case SHADER_PARAM::SP_TEXARR_7:
		{
			param.pData = new CResPtr<CTexture>;			
		}
		break;
		default:
			continue;
		}

		m_vecParam.push_back(param);
	}
}

void CMaterial::SetData(const SHADER_PARAM& _eParam, void * _pData)
{
	UINT idx = 0;
	bool bFind = false;
	for (; idx < m_vecParam.size(); ++idx)
	{
		if (m_vecParam[idx].eShaderParam == _eParam)
		{
			bFind = true;
			break;
		}
	}

	if (!bFind)
	{
		MessageBox(nullptr, L"쉐이더에 해당 인자가 추가되있지 않습니다", L"오류", MB_OK);
		assert(NULL);
		return;
	}

	switch (m_vecParam[idx].eShaderParam)
	{
	case SHADER_PARAM::SP_INT_0:
	case SHADER_PARAM::SP_INT_1:
	case SHADER_PARAM::SP_INT_2:
	case SHADER_PARAM::SP_INT_3:
	case SHADER_PARAM::SP_INT_4:
	case SHADER_PARAM::SP_INT_5:
	case SHADER_PARAM::SP_INT_6:
	case SHADER_PARAM::SP_INT_7:
	{
		*((int*)m_vecParam[idx].pData) = *((int*)_pData);
	}
	break;
	case SHADER_PARAM::SP_FLOAT_0:
	case SHADER_PARAM::SP_FLOAT_1:
	case SHADER_PARAM::SP_FLOAT_2:
	case SHADER_PARAM::SP_FLOAT_3:
	case SHADER_PARAM::SP_FLOAT_4:
	case SHADER_PARAM::SP_FLOAT_5:
	case SHADER_PARAM::SP_FLOAT_6:
	case SHADER_PARAM::SP_FLOAT_7:
	{
		*((float*)m_vecParam[idx].pData) = *((float*)_pData);
	}
	break;
	case SHADER_PARAM::SP_VEC4_0:
	case SHADER_PARAM::SP_VEC4_1:
	case SHADER_PARAM::SP_VEC4_2:
	case SHADER_PARAM::SP_VEC4_3:
	case SHADER_PARAM::SP_VEC4_4:
	case SHADER_PARAM::SP_VEC4_5:
	case SHADER_PARAM::SP_VEC4_6:
	case SHADER_PARAM::SP_VEC4_7:
	{
		*((Vec4*)m_vecParam[idx].pData) = *((Vec4*)_pData);
	}
	break;
	case SHADER_PARAM::SP_TEX_0:
	case SHADER_PARAM::SP_TEX_1:
	case SHADER_PARAM::SP_TEX_2:
	case SHADER_PARAM::SP_TEX_3:
	case SHADER_PARAM::SP_TEX_4:
	case SHADER_PARAM::SP_TEX_5:
	case SHADER_PARAM::SP_TEX_6:
	case SHADER_PARAM::SP_TEX_7:
	case SHADER_PARAM::SP_TEXARR_0:
	case SHADER_PARAM::SP_TEXARR_1:
	case SHADER_PARAM::SP_TEXARR_2:
	case SHADER_PARAM::SP_TEXARR_3:
	case SHADER_PARAM::SP_TEXARR_4:
	case SHADER_PARAM::SP_TEXARR_5:
	case SHADER_PARAM::SP_TEXARR_6:
	case SHADER_PARAM::SP_TEXARR_7:
	{
		*((CResPtr<CTexture>*)m_vecParam[idx].pData) = *((CTexture**)_pData);
	}
	default:
		break;
	}
}

void* CMaterial::GetData(const SHADER_PARAM& _eParam)
{
	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		if (_eParam == m_vecParam[i].eShaderParam)
			return m_vecParam[i].pData;
	}
	return NULL;
}

void CMaterial::UpdateData()
{
	if (NULL == m_pShader)
		assert(NULL);

	m_pShader->UpdateData();

	UpdateParam();	
}

void CMaterial::UpdateDataInstancing()
{
	if (NULL == m_pShader)
		assert(NULL);

	m_pShader->UpdateDataInstancing();

	UpdateParam();
}

void CMaterial::UpdateParam()
{
	int iArr[(UINT)SHADER_PARAM::SP_INT_END - (UINT)SHADER_PARAM::SP_INT_0] = {};
	float fArr[(UINT)SHADER_PARAM::SP_FLOAT_END - (UINT)SHADER_PARAM::SP_FLOAT_0] = {};
	Vec4 vecArr[(UINT)SHADER_PARAM::SP_VEC4_END - (UINT)SHADER_PARAM::SP_VEC4_0] = {};
	UINT TexCheck[(UINT)SHADER_PARAM::SP_TEX_END - (UINT)SHADER_PARAM::SP_TEX_0] = {};

	void* Arr[4] = { iArr , fArr , vecArr , TexCheck };

	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		switch (m_vecParam[i].eShaderParam)
		{
		case SHADER_PARAM::SP_INT_0:
		case SHADER_PARAM::SP_INT_1:
		case SHADER_PARAM::SP_INT_2:
		case SHADER_PARAM::SP_INT_3:
		case SHADER_PARAM::SP_INT_4:
		case SHADER_PARAM::SP_INT_5:
		case SHADER_PARAM::SP_INT_6:
		case SHADER_PARAM::SP_INT_7:
			iArr[(UINT)m_vecParam[i].eShaderParam - (UINT)SHADER_PARAM::SP_INT_0] = *((int*)m_vecParam[i].pData);
			break;
		case SHADER_PARAM::SP_FLOAT_0:
		case SHADER_PARAM::SP_FLOAT_1:
		case SHADER_PARAM::SP_FLOAT_2:
		case SHADER_PARAM::SP_FLOAT_3:
		case SHADER_PARAM::SP_FLOAT_4:
		case SHADER_PARAM::SP_FLOAT_5:
		case SHADER_PARAM::SP_FLOAT_6:
		case SHADER_PARAM::SP_FLOAT_7:
			fArr[(UINT)m_vecParam[i].eShaderParam - (UINT)SHADER_PARAM::SP_FLOAT_0] = *((float*)m_vecParam[i].pData);
			break;
		case SHADER_PARAM::SP_VEC4_0:
		case SHADER_PARAM::SP_VEC4_1:
		case SHADER_PARAM::SP_VEC4_2:
		case SHADER_PARAM::SP_VEC4_3:
		case SHADER_PARAM::SP_VEC4_4:
		case SHADER_PARAM::SP_VEC4_5:
		case SHADER_PARAM::SP_VEC4_6:
		case SHADER_PARAM::SP_VEC4_7:
			vecArr[(UINT)m_vecParam[i].eShaderParam - (UINT)SHADER_PARAM::SP_VEC4_0] = *((Vec4*)m_vecParam[i].pData);
			break;
		case SHADER_PARAM::SP_TEX_0:
		case SHADER_PARAM::SP_TEX_1:
		case SHADER_PARAM::SP_TEX_2:
		case SHADER_PARAM::SP_TEX_3:
		case SHADER_PARAM::SP_TEX_4:
		case SHADER_PARAM::SP_TEX_5:
		case SHADER_PARAM::SP_TEX_6:
		case SHADER_PARAM::SP_TEX_7:
		case SHADER_PARAM::SP_TEXARR_0:
		case SHADER_PARAM::SP_TEXARR_1:
		case SHADER_PARAM::SP_TEXARR_2:
		case SHADER_PARAM::SP_TEXARR_3:
		case SHADER_PARAM::SP_TEXARR_4:
		case SHADER_PARAM::SP_TEXARR_5:
		case SHADER_PARAM::SP_TEXARR_6:
		case SHADER_PARAM::SP_TEXARR_7:
			if (NULL != (*(CTexture**)m_vecParam[i].pData))
			{
				(*((CTexture**)m_vecParam[i].pData))->UpdateData(m_vecParam[i].iRegister, m_vecParam[i].iTiming);
				TexCheck[(UINT)m_vecParam[i].eShaderParam - (UINT)SHADER_PARAM::SP_TEX_0] = true;
			}
			break;
		}
	}

	static const CBUFFER* arrConstBuffer[4] =
	{
		CDevice::GetInst()->FindConstBuffer(g_SPName[(UINT)SHADER_PARAM::SP_INT_END]),
		CDevice::GetInst()->FindConstBuffer(g_SPName[(UINT)SHADER_PARAM::SP_FLOAT_END]),
		CDevice::GetInst()->FindConstBuffer(g_SPName[(UINT)SHADER_PARAM::SP_VEC4_END]),
		CDevice::GetInst()->FindConstBuffer(L"TexCheck")
	};

	for (int i = 0; i < 4; ++i)
	{
		D3D11_MAPPED_SUBRESOURCE tSub = {};

		CONTEXT->Map(arrConstBuffer[i]->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
		memcpy(tSub.pData, Arr[i], arrConstBuffer[i]->iSize);
		CONTEXT->Unmap(arrConstBuffer[i]->pBuffer, 0);

		CONTEXT->VSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
		CONTEXT->HSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
		CONTEXT->DSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
		CONTEXT->GSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
		CONTEXT->CSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
		CONTEXT->PSSetConstantBuffers(arrConstBuffer[i]->iRegister, 1, &arrConstBuffer[i]->pBuffer);
	}
}

void CMaterial::Save(FILE * _pFile)
{
	CRes::Save(_pFile);

	// 참조 쉐이더 유무
	BYTE bShader = 0;
	if (NULL != m_pShader)
		bShader = 1;
	fwrite(&bShader, 1, 1, _pFile);

	if (NULL != m_pShader)
	{
		// Shader key값 저장
		SaveWString(m_pShader->GetKey(), _pFile);
		
		for (UINT i = 0; i < m_vecParam.size(); ++i)
		{
			switch (m_vecParam[i].eShaderParam)
			{
			case SHADER_PARAM::SP_INT_0:
			case SHADER_PARAM::SP_INT_1:
			case SHADER_PARAM::SP_INT_2:
			case SHADER_PARAM::SP_INT_3:
			case SHADER_PARAM::SP_INT_4:
			case SHADER_PARAM::SP_INT_5:
			case SHADER_PARAM::SP_INT_6:
			case SHADER_PARAM::SP_INT_7:
			{				
				fwrite(m_vecParam[i].pData, sizeof(int), 1, _pFile);
			}
			break;
			case SHADER_PARAM::SP_FLOAT_0:
			case SHADER_PARAM::SP_FLOAT_1:
			case SHADER_PARAM::SP_FLOAT_2:
			case SHADER_PARAM::SP_FLOAT_3:
			case SHADER_PARAM::SP_FLOAT_4:
			case SHADER_PARAM::SP_FLOAT_5:
			case SHADER_PARAM::SP_FLOAT_6:
			case SHADER_PARAM::SP_FLOAT_7:
			{				
				fwrite(m_vecParam[i].pData, sizeof(float), 1, _pFile);
			}
			break;
			case SHADER_PARAM::SP_VEC4_0:
			case SHADER_PARAM::SP_VEC4_1:
			case SHADER_PARAM::SP_VEC4_2:
			case SHADER_PARAM::SP_VEC4_3:
			case SHADER_PARAM::SP_VEC4_4:
			case SHADER_PARAM::SP_VEC4_5:
			case SHADER_PARAM::SP_VEC4_6:
			case SHADER_PARAM::SP_VEC4_7:
			{				
				fwrite(m_vecParam[i].pData, sizeof(Vec4), 1, _pFile);
			}
			break;
			case SHADER_PARAM::SP_TEX_0:
			case SHADER_PARAM::SP_TEX_1:
			case SHADER_PARAM::SP_TEX_2:
			case SHADER_PARAM::SP_TEX_3:
			case SHADER_PARAM::SP_TEX_4:
			case SHADER_PARAM::SP_TEX_5:
			case SHADER_PARAM::SP_TEX_6:
			case SHADER_PARAM::SP_TEX_7:
			case SHADER_PARAM::SP_TEXARR_0:
			case SHADER_PARAM::SP_TEXARR_1:
			case SHADER_PARAM::SP_TEXARR_2:
			case SHADER_PARAM::SP_TEXARR_3:
			case SHADER_PARAM::SP_TEXARR_4:
			case SHADER_PARAM::SP_TEXARR_5:
			case SHADER_PARAM::SP_TEXARR_6:
			case SHADER_PARAM::SP_TEXARR_7:
			{				
				CTexture* pTex = *((CTexture**)m_vecParam[i].pData);

				BYTE bTex = 0;
				if (pTex != NULL)
					bTex = 1;

				fwrite(&bTex, 1, 1, _pFile);

				if (pTex)
					pTex->Save(_pFile);
			}
			}
		}
	}
}

CRes* CMaterial::Load(FILE * _pFile)
{
	wstring strKey = LoadWString(_pFile);
	wstring strPath = LoadWString(_pFile);

	return CResMgr::GetInst()->FindMaterial(strKey);
}

void CMaterial::LoadShaderParam(FILE * _pFile)
{
	// 참조 쉐이더 유무
	BYTE bShader = 0;
	if (NULL != m_pShader)
		bShader = 1;
	fread(&bShader, 1, 1, _pFile);

	if (!bShader)
		return;

	// Shader key값 저장
	wstring strShaderKey = LoadWString(_pFile);
	CShader* pShader = CShaderMgr::GetInst()->FindShader(strShaderKey);
	SetShader(pShader);
	
	for (UINT i = 0; i < m_vecParam.size(); ++i)
	{
		switch (m_vecParam[i].eShaderParam)
		{
		case SHADER_PARAM::SP_INT_0:
		case SHADER_PARAM::SP_INT_1:
		case SHADER_PARAM::SP_INT_2:
		case SHADER_PARAM::SP_INT_3:
		case SHADER_PARAM::SP_INT_4:
		case SHADER_PARAM::SP_INT_5:
		case SHADER_PARAM::SP_INT_6:
		case SHADER_PARAM::SP_INT_7:
		{			
			fread(m_vecParam[i].pData, sizeof(int), 1, _pFile);
		}
		break;
		case SHADER_PARAM::SP_FLOAT_0:
		case SHADER_PARAM::SP_FLOAT_1:
		case SHADER_PARAM::SP_FLOAT_2:
		case SHADER_PARAM::SP_FLOAT_3:
		case SHADER_PARAM::SP_FLOAT_4:
		case SHADER_PARAM::SP_FLOAT_5:
		case SHADER_PARAM::SP_FLOAT_6:
		case SHADER_PARAM::SP_FLOAT_7:
		{			
			fread(m_vecParam[i].pData, sizeof(float), 1, _pFile);
		}
		break;
		case SHADER_PARAM::SP_VEC4_0:
		case SHADER_PARAM::SP_VEC4_1:
		case SHADER_PARAM::SP_VEC4_2:
		case SHADER_PARAM::SP_VEC4_3:
		case SHADER_PARAM::SP_VEC4_4:
		case SHADER_PARAM::SP_VEC4_5:
		case SHADER_PARAM::SP_VEC4_6:
		case SHADER_PARAM::SP_VEC4_7:
		{			
			fread(m_vecParam[i].pData, sizeof(Vec4), 1, _pFile);
		}
		break;
		case SHADER_PARAM::SP_TEX_0:
		case SHADER_PARAM::SP_TEX_1:
		case SHADER_PARAM::SP_TEX_2:
		case SHADER_PARAM::SP_TEX_3:
		case SHADER_PARAM::SP_TEX_4:
		case SHADER_PARAM::SP_TEX_5:
		case SHADER_PARAM::SP_TEX_6:
		case SHADER_PARAM::SP_TEX_7:
		case SHADER_PARAM::SP_TEXARR_0:
		case SHADER_PARAM::SP_TEXARR_1:
		case SHADER_PARAM::SP_TEXARR_2:
		case SHADER_PARAM::SP_TEXARR_3:
		case SHADER_PARAM::SP_TEXARR_4:
		case SHADER_PARAM::SP_TEXARR_5:
		case SHADER_PARAM::SP_TEXARR_6:
		case SHADER_PARAM::SP_TEXARR_7:
		{				
			BYTE bTex = 0;
			fread(&bTex, 1, 1, _pFile);

			if (bTex)
			{
				wstring strKey = LoadWString(_pFile);
				wstring strPath = LoadWString(_pFile);
				CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey, strPath);
				(*(CResPtr<CTexture>*)m_vecParam[i].pData) = pTex;
			}
		}
		}
	}
}
