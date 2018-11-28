#include "WaterScript.h"

#include "TimeMgr.h"
#include "SceneMgr.h"
#include "KeyMgr.h"

CWaterScript::CWaterScript()
	: m_fDelta(0.f)
	, m_vCamWorld(0.f, 100.f, 0.f, 0.f)
{
}

CWaterScript::~CWaterScript()
{
}

void CWaterScript::Start()
{
}

int CWaterScript::Update()
{
	m_fDelta = CTimeMgr::GetInst()->GetTimer();

	CMaterial* pMtrl = MeshRender()->GetSharedMaterial();
	pMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &m_fDelta);

	CGameObject* pCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);
	Vec3 vPos = Transform()->GetLocalPos();

	m_vCamWorld = pCamObj->Transform()->GetWorldPos().GetVec4();
	m_vCamWorld.y = vPos.y + 100.36f;

	/*if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_Q, STATE_HOLD))
		m_vCamWorld.y += 10 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_E, STATE_HOLD))
		m_vCamWorld.y -= 10 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_R, STATE_HOLD))
		m_vCamWorld.x += 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_T, STATE_HOLD))
		m_vCamWorld.x -= 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_Y, STATE_HOLD))
		m_vCamWorld.z += 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_U, STATE_HOLD))
		m_vCamWorld.z -= 1000 * DT();*/

	pMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &m_vCamWorld);

	return 0;
}

int CWaterScript::LateUpdate()
{
	return 0;
}

int CWaterScript::DelayUpdate()
{
	m_fDelta = CTimeMgr::GetInst()->GetTimer();

	CMaterial* pMtrl = MeshRender()->GetSharedMaterial();
	pMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &m_fDelta);

	CGameObject* pCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);

	m_vCamWorld = pCamObj->Transform()->GetWorldPos().GetVec4();
	m_vCamWorld.y = 100.36f;

	/*if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_Q, STATE_HOLD))	
		m_vCamWorld.y += 10 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_E, STATE_HOLD))
		m_vCamWorld.y -= 10 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_R, STATE_HOLD))
		m_vCamWorld.x += 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_T, STATE_HOLD))
		m_vCamWorld.x -= 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_Y, STATE_HOLD))
		m_vCamWorld.z += 1000 * DT();

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_U, STATE_HOLD))
		m_vCamWorld.z -= 1000 * DT();*/

	pMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &m_vCamWorld);

	return 0;
}

