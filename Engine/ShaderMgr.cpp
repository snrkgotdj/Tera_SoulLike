#include "ShaderMgr.h"

#include "PathMgr.h"
#include "Shader.h"

#include "RenderMgr.h"

CShaderMgr::CShaderMgr()
{
}

CShaderMgr::~CShaderMgr()
{
	Safe_Delete_Map(m_mapShader);
}

void CShaderMgr::init()
{
	CreateShader();		
}

void CShaderMgr::CreateShader()
{
	//===============
	// Color Shader
	//===============	
	CShader* pShader = new CShader;

	wstring strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Color", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Color_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Color", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_Color", pShader);

	//===============
	// Texture Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Tex", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Tex_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Tex", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");	
	
	CShaderMgr::GetInst()->AddShader(L"Shader_Tex", pShader);

	//===============
	// Collider Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Collider", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Collider_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Collider", 5, 0);
	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL);
		
	CShaderMgr::GetInst()->AddShader(L"Shader_Collider", pShader);

	//===============
	// Std 2D Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Std_2D", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Std_2D_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Std_2D", 5, 0);
	
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL2"));
		
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_Std2D", pShader);

	//===============
	// Std 2D OneBlend Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Std_2D", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Std_2D_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Std_2D", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"OneBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL2"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_Std2D_OneBlend", pShader);

	//===============
	// Std 3d Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Std_3D", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Std_3D_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Std_3D", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Noraml Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"SHADOW MAP TARGET");

	CShaderMgr::GetInst()->AddShader(L"Shader_Std3D", pShader);

	//=====================
	// Std Deferred Shader 
	//=====================
	pShader = new CShader;
	pShader->SetDeferred();

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Deferred.fx";

	pShader->CreateVertexShader(strPath, "VS_Std_Deferred", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Std_Deferred_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Std_Deferred", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Normal Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"Specular Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_3, (UINT)SHADER_TYPE::ST_PIXEL, L"Emissive Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_StdDeferred", pShader);

	//===============
	// Sky Box Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Sky", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Sky", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");

	CShaderMgr::GetInst()->AddShader(L"Shader_Sky", pShader);

	//===============
	// Sky Box Cube
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Sky_Cube", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Sky_Cube", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEXARR_7, (UINT)SHADER_TYPE::ST_PIXEL, L"DDS Texture");

	CShaderMgr::GetInst()->AddShader(L"Shader_SkyCube", pShader);

	//===============
	// Terrain Shader
	//===============	
	pShader = new CShader;
	pShader->SetDeferred();

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Terrain.fx";

	pShader->CreateVertexShader(strPath, "VS_Terrain", 5, 0);
	pShader->CreateHullShader(strPath, "HS", 5, 0);
	pShader->CreateDomainShader(strPath, "DS", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Terrain", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_PIXEL);

	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_PIXEL | (UINT)SHADER_TYPE::ST_HULL, L"USING TESSELLATION");
	pShader->AddShaderParam(SHADER_PARAM::SP_INT_1, (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_PIXEL | (UINT)SHADER_TYPE::ST_HULL, L"Terrain Mod");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_VERTEX, L"X Face Count");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_1, (UINT)SHADER_TYPE::ST_VERTEX, L"Z Face Count");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_0, (UINT)SHADER_TYPE::ST_VERTEX, L"Brush Ratio");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_1, (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_2, (UINT)SHADER_TYPE::ST_HULL, L"Main Camera Position");

	pShader->AddShaderParam(SHADER_PARAM::SP_TEXARR_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DIFFUSE TEX ARR");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEXARR_1, (UINT)SHADER_TYPE::ST_PIXEL, L"NORMAL TEX ARR");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_DOMAIN | (UINT)SHADER_TYPE::ST_PIXEL, L"HEIGHTMAP TEX");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"WEIGHT TEX");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"BRUSH TEX");

	CShaderMgr::GetInst()->AddShader(L"Shader_Terrain", pShader);

	//====================
	// Tessellation Shader
	//====================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Tessellation.fx";

	pShader->CreateVertexShader(strPath, "VS", 5, 0);
	pShader->CreateHullShader(strPath, "HS", 5, 0);
	pShader->CreateDomainShader(strPath, "DS", 5, 0);
	pShader->CreatePixelShader(strPath, "PS", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_HULL);
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DIFFUSE TEX");

	CShaderMgr::GetInst()->AddShader(L"Shader_Tessellation", pShader);

	//=========================
	// Gaussian Filter Shader
	//=========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Filter.fx";

	pShader->CreateVertexShader(strPath, "VS_Gaussian", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Gaussian", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"StepU");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_1, (UINT)SHADER_TYPE::ST_PIXEL, L"StepV");

	CShaderMgr::GetInst()->AddShader(L"Shader_Gaussian", pShader);

	//=========================
	// Distortion Filter Shader
	//=========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Filter.fx";

	pShader->CreateVertexShader(strPath, "VS_Distortion", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Distortion", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");

	CShaderMgr::GetInst()->AddShader(L"Shader_Distortion", pShader);
	
	//=========================
	// Detort Filter Shader	
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\Filter.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_Detort", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Detort", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	AddShader(L"Shader_Detort", pShader);
	
	//=========================
	// OldFilm Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\Filter.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_OldFilm", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_OldFilm", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	AddShader(L"Shader_OldFilm", pShader);
	
	//=========================
	// UnderWater Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\Filter.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_OldFilm", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_UnderWater", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Timer");
	AddShader(L"Shader_UnderWater", pShader);

	//=========================
	// Ripple Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\Filter.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_OldFilm", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Ripple", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Timer");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Timer2");
	AddShader(L"Shader_Ripple", pShader);

	//=========================
	// DepthOfField Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\DepthOfField.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_DepthOfField", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_DepthOfField", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Post Target");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Depth Target");

	AddShader(L"Shader_DepthOfField", pShader);

	//=========================
	// Fog Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\Fog.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_Fog", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Fog", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Post Target");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Depth Target");

	AddShader(L"Shader_Fog", pShader);

	//=========================
	// BloodScreen Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\BloodScreen.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_BloodScreen", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_BloodScreen", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Post Target");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"BloodScreen Texture");

	AddShader(L"Shader_BloodScreen", pShader);

	//=========================
	// Ripple Filter Shader
	//=========================
	strPath = CPathMgr::GetResPath();
	strPath += L"\\Shader\\MotionBlur.fx";

	pShader = new CShader;
	pShader->CreateVertexShader(strPath, "VS_MOTIONBLUR", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_MOTIONBLUR", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Velocity Target");
	AddShader(L"Shader_MotionBlur", pShader);

	//=====================
	// Fade Filter Shader
	//=====================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Filter.fx";

	pShader->CreateVertexShader(strPath, "VS_OldFilm", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_FADE", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Timer");

	CShaderMgr::GetInst()->AddShader(L"Shader_Fade", pShader);

	//===================
	// Volume Mesh Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Light.fx";

	pShader->CreateVertexShader(strPath, "VS_Volume", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Volume", 5, 0);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"ColorWriteDisable"));

	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"asdasdg");

	CShaderMgr::GetInst()->AddShader(L"Shader_Volume", pShader);

	//===================
	// HDR Pass1 Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostDownScale.fx";

	pShader->CreateComputeShader(strPath, "DownScaleFirstPass", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_HDR_Pass1", pShader);

	//===================
	// HDR Pass2 Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostDownScale.fx";

	pShader->CreateComputeShader(strPath, "DownScaleSecondPass", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_HDR_Pass2", pShader);

	//===================
	// Bloom Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostDownScale.fx";

	pShader->CreateComputeShader(strPath, "BloomReveal", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_Bloom", pShader);

	//=========================
	// Blur_Horizontal Shader
	//=========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Blur.fx";

	pShader->CreateComputeShader(strPath, "HorizFilter", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_Blur_Horizontal", pShader);

	//=========================
	// Blur_Vertical Shader
	//=========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Blur.fx";

	pShader->CreateComputeShader(strPath, "VerticalFilter", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_Blur_Vertica", pShader);

	//===================
	// HDR Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\HDR.fx";

	pShader->CreateVertexShader(strPath, "VS_HDR", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_HDR", 5, 0);

	CShaderMgr::GetInst()->AddShader(L"Shader_HDR", pShader);

	//==================================
	// Light Range Sphere Shader Shader
	//==================================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Tool.fx";

	pShader->CreateVertexShader(strPath, "VS_LR_Sphere", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_LR_Sphere", 5, 0);

	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));
	CShaderMgr::GetInst()->AddShader(L"Shader_RangeSphere", pShader);

	//===================
	// Light Draw Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Light.fx";

	pShader->CreateVertexShader(strPath, "VS_LightDraw", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_LightDraw", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"OneBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"STENCIL_PASS"), 1);

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"POSITION TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"NORMAL TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"DEPTH TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_3, (UINT)SHADER_TYPE::ST_PIXEL, L"SHADOW MAP TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_4, (UINT)SHADER_TYPE::ST_PIXEL, L"SPECULAR MAP TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_5, (UINT)SHADER_TYPE::ST_PIXEL, L"Emissive MAP TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Light Index");

	CShaderMgr::GetInst()->AddShader(L"Shader_LightDraw", pShader);

	//===================
	// Light Merge Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Light.fx";

	pShader->CreateVertexShader(strPath, "VS_LightMerge", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_LightMerge", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS_NONE"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DIFFUSE TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"LIGHT TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"SPECULAR TARGET");

	CShaderMgr::GetInst()->AddShader(L"Shader_LightMerge", pShader);

	//===================
	// Button Shader
	//===================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\UI.fx";

	pShader->CreateVertexShader(strPath, "VS_BUTTON", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_BUTTON_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_BUTTON", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"SATE");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"IMAGE");

	CShaderMgr::GetInst()->AddShader(L"Shader_Button", pShader);

	//=====================
	// NavMesh Shader 
	//=====================
	pShader = new CShader;
	pShader->SetDeferred();

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\NavMesh.fx";

	pShader->CreateVertexShader(strPath, "VS_NavMesh", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_NavMesh_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_NavMesh", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Normal Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"MESH FLAGE");

	CShaderMgr::GetInst()->AddShader(L"Shader_NavMesh", pShader);

	// Åø ½¦ÀÌ´õ

	//===============
	// Grid Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Tool.fx";

	pShader->CreateVertexShader(strPath, "VS_Grid", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Grid", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_1, (UINT)SHADER_TYPE::ST_PIXEL);

	CShaderMgr::GetInst()->AddShader(L"Shader_Grid", pShader);

	//===============
	// TOOL UI Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Tool.fx";

	pShader->CreateVertexShader(strPath, "VS_UI", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_UI", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_0, (UINT)SHADER_TYPE::ST_VERTEX);
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_1, (UINT)SHADER_TYPE::ST_VERTEX);
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_2, (UINT)SHADER_TYPE::ST_VERTEX);
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_3, (UINT)SHADER_TYPE::ST_VERTEX);

	CShaderMgr::GetInst()->AddShader(L"Shader_ToolUI", pShader);



	//=================
	// Dir Shadow Map
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Light.fx";

	pShader->CreateVertexShader(strPath, "VS_DirShadowMap", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_DirShadowMap_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_DirShadowMap", 5, 0);

	pShader->AddShaderParam(SHADER_PARAM::SP_INT_0, (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);

	CShaderMgr::GetInst()->AddShader(L"Shader_DirShadowMap", pShader);

	//=================
	// Black Effect Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_POST", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_POST_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_BlackEffect", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_BlackEffect", pShader);

	//=================
	// Test Geometry Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Geometry.fx";

	pShader->CreateVertexShader(strPath, "VS_TestGeo", 5, 0);
	pShader->CreateGeometryShader(strPath, "GS_TestGeo", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_TestGeo", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	CShaderMgr::GetInst()->AddShader(L"Shader_TestGeo", pShader);

	//=================
	// StreamOut Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Particle.fx";

	pShader->CreateVertexShader(strPath, "VS_StreamOut", 5, 0);
	pShader->CreateGeometryShader(strPath, "GS_StreamOut", 5, 0);

	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"DISABLE"));

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_GEOMETRY, L"Delta Time");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_0, (UINT)SHADER_TYPE::ST_GEOMETRY, L"vRandom");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_1, (UINT)SHADER_TYPE::ST_GEOMETRY, L"Emit Pos");

	CShaderMgr::GetInst()->AddShader(L"Shader_StreamOut", pShader);

	//=================
	// DrawParticle Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Particle.fx";

	pShader->CreateVertexShader(strPath, "VS_ParticleDraw", 5, 0);
	pShader->CreateGeometryShader(strPath, "GS_ParticleDraw", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_ParticleDraw", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_0, (UINT)SHADER_TYPE::ST_GEOMETRY, L"vCamAxisX");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_1, (UINT)SHADER_TYPE::ST_GEOMETRY, L"vCamAxisY");

	CShaderMgr::GetInst()->AddShader(L"Shader_DrawParticle", pShader);

	//=================
	// Water Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_Water", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Water", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Delta Time");
	pShader->AddShaderParam(SHADER_PARAM::SP_VEC4_0, (UINT)SHADER_TYPE::ST_PIXEL, L"vCamPos");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Water Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"Water Normal");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_3, (UINT)SHADER_TYPE::ST_PIXEL, L"Depth Target");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEXARR_7, (UINT)SHADER_TYPE::ST_PIXEL, L"World Cube");

	CShaderMgr::GetInst()->AddShader(L"Shader_Water", pShader);

	//===============
	// Tile Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_Water", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Tile_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Tile", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL2"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Noraml Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"POST Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Tile_Effect", pShader);

	//=================
	// Trail Shader
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Trail.fx";

	pShader->CreateVertexShader(strPath, "VS_Trail", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Trail", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Trail Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"Noise Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Trail", pShader);

	//=================
	// Trail Shader Alpha
	//=================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Trail.fx";

	pShader->CreateVertexShader(strPath, "VS_Trail", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Trail_Alpha", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Trail Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"Noise Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Trail_Alpha", pShader);

	//==========================
	// Distortion Effect Shader
	//==========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_POST", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_POST_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Distortion_Effect", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_Distortion_Effect", pShader);

	//==========================
	// Electric Effect Shader
	//==========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_POST", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_POST_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Electric_Effect", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"EFFECT Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"WAVE Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Electric_Effect", pShader);

	//==========================
	// Wave Effect Shader
	//==========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_POST", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_POST_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_WAVE_Effect", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"WAVE Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"WAVE Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Wave_Effect", pShader);

	//==========================
	// Potal Effect Shader
	//==========================
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\PostEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_POST", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_POST_Inst", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_Potal_Effect", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"FOWARD Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	CShaderMgr::GetInst()->AddShader(L"Shader_Potal_Effect", pShader);

	//===============
	// Dissolve Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\BurnOut.fx";

	pShader->CreateVertexShader(strPath, "VS_BurnOut", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_BurnOut_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_BurnOut", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"fTime");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Noraml Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_2, (UINT)SHADER_TYPE::ST_PIXEL, L"SHADOW MAP TARGET");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_3, (UINT)SHADER_TYPE::ST_PIXEL, L"Noise Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_BurnOut", pShader);

	//===============
	// AfterImage Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\AfterImageEffect.fx";

	pShader->CreateVertexShader(strPath, "VS_AfterImage", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_AfterImage_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_AfterImage", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"fTime");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Diffuse Tex");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Noise Tex");

	CShaderMgr::GetInst()->AddShader(L"Shader_AfterImage", pShader);

	//===============
	// SoftParticle Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\SoftParticle.fx";

	pShader->CreateVertexShader(strPath, "VS_SoftParticle", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_SoftParticle_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_SoftParticle", 5, 0);

	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"OneBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"ALWAYS"));

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Depth Texture");

	CShaderMgr::GetInst()->AddShader(L"Shader_SoftParticle", pShader);

	//===============
	// HP Shader
	//===============	
	pShader = new CShader;

	strPath = CPathMgr::GetResPath();
	strPath += L"Shader\\Standard.fx";

	pShader->CreateVertexShader(strPath, "VS_Tex", 5, 0);
	pShader->CreateVertexShader_Instancing(strPath, "VS_Tex_Instancing", 5, 0);
	pShader->CreatePixelShader(strPath, "PS_HP", 5, 0);
	pShader->AddSampler(CRenderMgr::GetInst()->FindSampler(L"DefaultSampler"), (UINT)SHADER_TYPE::ST_PIXEL);
	pShader->SetBlendState(CRenderMgr::GetInst()->FindBlendState(L"AlphaBlend"));
	pShader->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"LESS_EQUAL"));

	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Alpha");
	pShader->AddShaderParam(SHADER_PARAM::SP_FLOAT_1, (UINT)SHADER_TYPE::ST_PIXEL, L"DT");

	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_0, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");
	pShader->AddShaderParam(SHADER_PARAM::SP_TEX_1, (UINT)SHADER_TYPE::ST_PIXEL, L"Texture");

	CShaderMgr::GetInst()->AddShader(L"Shader_HP", pShader);
}


int CShaderMgr::AddShader(const wstring& _strKey, CShader * _pShader)
{
	CShader* pShader = FindShader(_strKey);

	if (NULL != pShader)
		return RET_FAILED;

	_pShader->SetKey(_strKey);
	m_mapShader.insert(make_pair(_strKey, _pShader));

	return RET_SUCCESS;
}


CShader * CShaderMgr::FindShader(const wstring& _strKey)
{
	map<wstring, CShader*>::iterator iter = m_mapShader.find(_strKey);

	if (iter == m_mapShader.end())
		return NULL;

	return iter->second;
}