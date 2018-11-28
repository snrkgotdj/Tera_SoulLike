#include "ThirdCamScript.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "Texture.h"

#include "ParentScript\PlayerScript.h"

CThirdCamScript::CThirdCamScript()
	: m_pTargetObj(NULL)
	, m_fDistance(1000.f)
	, m_fHeight(100.f)
	, m_fRight(0.f)
	, m_fWalkSpeed(200)
	, m_fRunSpeed(600)
	, m_eState(CAMERA_STATE::END)
	, m_bVibe(false)
{
	m_ID = SCRIPT_THIRDCAMERA;
}

CThirdCamScript::~CThirdCamScript()
{
}

void CThirdCamScript::Awake()
{
	m_fDistance = 240.f;
	m_fHeight = 80.f;
	m_fRight = 0.f;

	Camera()->SetFar(100000.f);

	m_pTargetObj = CSceneMgr::GetInst()->GetPlayerObj();

	SetState(CAMERA_STATE::FOLLOW);

	//Vec3 vPos = m_pTargetObj->Transform()->GetLocalPos();
	//vPos += m_pTargetObj->Transform()->GetDir(DIR_TYPE::RIGHT) * -m_fDistance;
	//vPos.y += m_fHeight;
	//Transform()->SetLocalPos(vPos);

	//ShowCursor(false);
}

int CThirdCamScript::Update()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vRot = Transform()->GetLocalRot();

	Vec3 vLook = m_pTargetObj->Transform()->GetLocalPos();

	Vec3 vDir = vLook - vPos;
	Vec3 vDist = vDir;

	vDir.Normalize();
	vDir.y = 0.f;

	if(CAMERA_STATE::FOLLOW == m_eState)
	{
		float fDist = vDist.Distance();
		if (fDist > m_fDistance)
			vPos += vDir * 150.f * DT();
		
		else if (fDist < m_fDistance - 10)
			vPos -= vDir * 150.f * DT();
		
		if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_RBTN, KEY_STATE::STATE_HOLD))
		{		
			Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

			float fAngle = vMouseDir.x * DT();
			Matrix matRot = XMMatrixRotationY(fAngle);

			fAngle = -vMouseDir.y * DT();
			matRot *= XMMatrixRotationAxis(Transform()->GetDir(DIR_TYPE::RIGHT).Convert(), fAngle);

			memcpy(&matRot._41, &vLook, sizeof(Vec3));
			vPos = XMVector3TransformCoord(-vDist.Convert(), matRot);
		}

		vLook.y += m_fHeight;
		vLook -= m_pTargetObj->Transform()->GetDir(DIR_TYPE::FRONT) * m_fRight;

		Matrix matView = XMMatrixLookAtLH(vPos.Convert(), vLook.Convert(), m_pTargetObj->Transform()->GetDir(DIR_TYPE::UP).Convert());
		Camera()->SetLookAtMat(matView);

		vRot.y = atan2(vDir.x, vDir.z);
	}

	if (m_bVibe)
	{
		float fPosX = getRandNum_float(-1.f, 1.f);
		float fPosY = getRandNum_float(-1.f, 1.f);
		float fPosZ = getRandNum_float(-1.f, 1.f);

		Vec3 vDir = Vec3();

		if (getRandNum_int(0, 1))
		{
			vDir = m_pTargetObj->Transform()->GetDir(DIR_TYPE::FRONT);
		}
		else
		{
			vDir = m_pTargetObj->Transform()->GetDir(DIR_TYPE::UP);
		}

		vPos += vDir * Vec3(fPosX, fPosY, fPosZ) * m_fPower * DT();
	}

	Transform()->SetLocalPos(vPos);
	Transform()->SetLocalRot(vRot);

	return 0;
}

void CThirdCamScript::GetPos(const PLAYER_STATE & _eState, const DIR_STATE& _eDir, Vec3& _vOutPos)
{
	Vec3 vDir;
	Vec3 vVelo;

	switch (_eDir)
	{
	case DIR_STATE::FRONT:
		vDir = Transform()->GetDir(DIR_TYPE::FRONT);
		break;
	case DIR_STATE::BACK:
		vDir = Transform()->GetDir(DIR_TYPE::FRONT) * -1;
		break;
	case DIR_STATE::RIGHT:
		vDir = Transform()->GetDir(DIR_TYPE::RIGHT);
		break;
	case DIR_STATE::LEFT:
		vDir = Transform()->GetDir(DIR_TYPE::RIGHT) * -1;
		break;
	case DIR_STATE::END:
		vDir = Vec3();
		break;
	}

	switch (_eState)
	{
	case WALK:
		vVelo = vDir * m_fWalkSpeed * DT();
		break;
	case RUN:
		vVelo = vDir * m_fRunSpeed * DT();
		break;
	}

	vVelo.y = 0;

	_vOutPos = vVelo;
}

void CThirdCamScript::GetDir(Vec3 & _vPos)
{
	Vec3 vRot = Transform()->GetLocalRot();
	_vPos = vRot;
}

void CThirdCamScript::SetVibration(bool _bVibe, float _fPower,  bool _bMust)
{
	if (m_bVibe == _bVibe)
		return;

	m_bVibe = _bVibe;
	m_fPower = _fPower;

	if (m_bVibe)
	{
		m_vPrevPos = Transform()->GetLocalPos();
	}
	else
	{
		if (_bMust)
		{
			Transform()->SetLocalPos(m_vPrevPos);
		}
		else
		{
			Vec3 vTargetPos = m_pTargetObj->Transform()->GetLocalPos();
			Vec3 vDist = vTargetPos - m_vPrevPos;
			float fDist = vDist.Distance();

			if (fDist <= m_fDistance)
				Transform()->SetLocalPos(m_vPrevPos);
		}
	}
}

void CThirdCamScript::SetState(const CAMERA_STATE& _eState)
{
	if (m_eState == _eState)
		return;

	m_eState = _eState;
	Vec3 vPos;

	switch (m_eState)
	{
	case CAMERA_STATE::FOLLOW:
		Camera()->SetLookAt(true);
		vPos = m_pTargetObj->Transform()->GetLocalPos();
		vPos += m_pTargetObj->Transform()->GetDir(DIR_TYPE::RIGHT) * -m_fDistance;
		vPos.y += m_fHeight;
		Transform()->SetLocalPos(vPos);
		break;
	case CAMERA_STATE::FREE:
		Camera()->SetLookAt(false);
		break;
	}
}

void CThirdCamScript::Save(FILE * _pFile)
{
	fwrite(&m_fDistance, sizeof(float), 1, _pFile);
	fwrite(&m_fHeight, sizeof(float), 1, _pFile);
}

void CThirdCamScript::Load(FILE * _pFile)
{
	fread(&m_fDistance, sizeof(float), 1, _pFile);
	fread(&m_fHeight, sizeof(float), 1, _pFile);
}