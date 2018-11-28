#include "TerrainScript.h"

#include "SceneMgr.h"

CTerrainScript::CTerrainScript()
{
}


CTerrainScript::~CTerrainScript()
{
}

int CTerrainScript::Update()
{
	CGameObject* pCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);
	Vec3 vPos = pCam->Transform()->GetLocalPos();
	Vec4 vCamPos = Vec4(vPos.x, vPos.y, vPos.z, 0.f);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_VEC4_2, &vCamPos);

	return 0;
}

int CTerrainScript::DelayUpdate()
{
	CGameObject* pCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
	Vec3 vPos = pCam->Transform()->GetLocalPos();
	Vec4 vCamPos = Vec4(vPos.x, vPos.y, vPos.z, 0.f);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_VEC4_2, &vCamPos);

	return 0;
}