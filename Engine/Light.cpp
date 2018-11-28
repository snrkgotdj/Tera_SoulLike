#include "Light.h"

#include "Transform.h"
#include "CamFilter.h"
#include "RenderMgr.h"
#include "Device.h"
#include "DepthStencilState.h"
#include "Material.h"
#include "Shader.h"

#include "ResMgr.h"
#include "Camera.h"

CCamFilter* CLight::g_pLightFilter = NULL;

CLight::CLight()
	: m_tInfo()
	, m_pVolumeFilter(NULL)
	, m_iIdx(0)
	, m_fRange(1.f)
{
	m_LightCam.SetAllLayerCheck();
	m_LightCam.SetCameraType(CAMERA_TYPE::LIGHT);
}

CLight::CLight(const tLightInfo & _info)
	: m_tInfo()
	, m_pVolumeFilter(NULL)
	, m_iIdx(0)
	, m_fRange(1.f)
{
	SetLightInfo(_info);
}

CLight::~CLight()
{

}

int CLight::Update()
{
	return 0;
}

int CLight::FinalUpdate()
{
	// m_LightCam.m_bPerspective = false;
	m_LightCam.CreateViewMat(Transform());
	m_LightCam.CreateProjMat();
	m_tInfo.matVP = XMMatrixTranspose(m_LightCam.m_matView * m_LightCam.m_matProj);

	m_LightCam.m_bPerspective = true;
	//m_LightCam.UpdateInstancing_shadow();
	return 0;
}

void CLight::Render()
{
	if (m_tInfo.iLightType != (UINT)LIGHT_TYPE::DIR)
	{
		g_pLightFilter->GetMaterial()->GetShader()->SetStencilRef(1);

		// VolumeFilter 에 Transform 정보 업데이트
		if (m_tInfo.iLightType == (UINT)LIGHT_TYPE::POINT)
		{
			float fDist = m_tInfo.vLightDir.Distance();
			m_pVolumeFilter->SetScale(Vec3(fDist, fDist, fDist));
			m_pVolumeFilter->SetPos(Vec3(m_tInfo.vLightPos.x , m_tInfo.vLightPos.y, m_tInfo.vLightPos.z));
		}
		else
		{

		}

		m_pVolumeFilter->FinalUpdate();

		// 후면보다 앞에 있을 때
		//int iData = 0;
		//m_pVolumeFilter->GetMaterial()->SetData(SHADER_PARAM::SP_INT_0, &iData);

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_FRONT);
		m_pVolumeFilter->GetMaterial()->GetShader()->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"BEFORE_BACK"), 1);
		m_pVolumeFilter->Render();

		// 전면보다 뒤에 있을 때
		//iData = 1;
		//m_pVolumeFilter->GetMaterial()->SetData(SHADER_PARAM::SP_INT_0, &iData);

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		m_pVolumeFilter->GetMaterial()->GetShader()->SetDepthStencilState(CRenderMgr::GetInst()->FindDepthStencilState(L"AFTER_FRONT"), 1);
		m_pVolumeFilter->Render();
	}
	else
	{
		m_pVolumeFilter = NULL;
		g_pLightFilter->GetMaterial()->GetShader()->SetStencilRef(0);
	}

	// 검출된 영역에 대해서 빛을 그린다.
	g_pLightFilter->GetMaterial()->SetData(SHADER_PARAM::SP_INT_0, &m_iIdx);
	g_pLightFilter->GetMaterial()->SetData(SHADER_PARAM::SP_TEX_3, &m_pShadowMap);
	m_pStd3DMtrl->SetData(SHADER_PARAM::SP_TEX_2, &m_pShadowMap);

	g_pLightFilter->Render();
}

void CLight::RangeRender()
{
	if (NULL != m_pVolumeFilter)
		m_pVolumeFilter->RangeRender();
}

void CLight::render_shadowmap()
{
	m_pShadowMtrl->SetData(SHADER_PARAM::SP_INT_0, &m_iIdx);
	m_LightCam.Render_ShadowMap(m_pShadowMtrl);
}

void CLight::SetLightInfo(const tLightInfo & _tLightInfo)
{
	m_tInfo = _tLightInfo;
	Vec3 vPos = Vec3(m_tInfo.vLightPos.x, m_tInfo.vLightPos.y, m_tInfo.vLightPos.z);
	Transform()->SetLocalPos(vPos);

	if (m_tInfo.vLightDir.Distance() > 0)
		m_tInfo.vLightDir.Normalize();
	Matrix matRot = GetRotMatirx(m_tInfo.vLightDir);

	Transform()->SetLocalRotMat(matRot);

	SetRange(m_fRange);

	if ((UINT)LIGHT_TYPE::POINT == m_tInfo.iLightType)
	{
		m_pVolumeFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_PointLight");
	}
	else if ((UINT)LIGHT_TYPE::SPOT == m_tInfo.iLightType)
	{
		m_pVolumeFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_SpotLight");
	}
	// Direction Light일때 그림자
	else if((UINT)LIGHT_TYPE::DIR == m_tInfo.iLightType)
	{
		m_pShadowMap = CResMgr::GetInst()->FindTexture(L"RenderTarget_8");
		m_pShadowMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_DirShadowMap");
		m_LightCam.SetPerspective(true);
		m_LightCam.SetWidth(m_pShadowMap->GetWidth());
		m_LightCam.SetHeight(m_pShadowMap->GetHeight());
		m_LightCam.SetNear(1.f);
		m_LightCam.SetFar(1000000.f);
	}

	m_pStd3DMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Std3D");
}

void CLight::SetRange(float _fRange)
{
	m_fRange = _fRange;
	m_tInfo.vLightDir.Normalize();
	m_tInfo.vLightDir *= m_fRange;
}

void CLight::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::LIGHT;
	fwrite(&iType, sizeof(UINT), 1, _pFile);
	fwrite(&m_tInfo, sizeof(tLightInfo), 1, _pFile);
}

void CLight::Load(FILE * _pFile)
{
	fread(&m_tInfo, sizeof(tLightInfo), 1, _pFile);
	SetLightInfo(m_tInfo);
}