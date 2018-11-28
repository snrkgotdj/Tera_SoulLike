#include "GridScript.h"

#include "Scene.h"
#include "SceneMgr.h"

CGridScript::CGridScript()
	: m_pToolCam(NULL)
	, m_iMinStep(100)
	, m_iMaxStep(10000)
	, m_fBoldRatio(0.004f)
{
}

CGridScript::~CGridScript()
{
}

int CGridScript::Update()
{
	FindToolCam();

	if (NULL == m_pToolCam)
		return 0;

	Vec3 vLocalPos = m_pToolCam->Transform()->GetLocalPos();
	Transform()->SetLocalPos(Vec3(vLocalPos.x, 0.f, vLocalPos.z));

	CalGridStep();

	return 0;
}

void CGridScript::FindToolCam()
{
	if (NULL != m_pToolCam)
		return;

	m_pToolCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
}

void CGridScript::CalGridStep()
{
	int iCamY = abs((int)m_pToolCam->Transform()->GetLocalPos().y);
	int iStep = m_iMinStep;
	int i = iStep;

	for (; i <= m_iMaxStep; i *= 10)
	{
		if (iCamY <= i * 10)
		{
			break;
		}
	}
	iStep = i;

	// Cal Bold 
	float fBold = abs((float)iCamY * m_fBoldRatio);

	// Cal Alpha
	float fAlpha = 1.f;
	if (iStep * 5 < iCamY)
	{
		fAlpha = abs((float)(iCamY - (iStep * 10)) / (float)(iStep * 5));
	}

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_INT_0, &iStep);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fAlpha);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &fBold);
}