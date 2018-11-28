#include "BillBoardScript.h"
#include "Transform.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"


CBillBoardScript::CBillBoardScript()
	: m_bAll(false)
{
#ifdef _DEBUG
	m_bOnce = false;
#endif // DEBUG
	m_ID = SCRIPT_BILLBOARD;
}


CBillBoardScript::~CBillBoardScript()
{
}


void CBillBoardScript::Start()
{
	Transform()->SetBillBoard();
}

int CBillBoardScript::Update()
{
	return 0;
}

int CBillBoardScript::LateUpdate()
{
	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vScale = Transform()->GetLocalScale();

	CGameObject* pCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);

	XMMATRIX matView = XMMatrixIdentity();
	
	if (false == m_bAll)
	{
		XMMATRIX matCamView = pCam->Camera()->GetViewMat();
		matView._11 = matCamView._11;
		matView._13 = matCamView._13;
		matView._31 = matCamView._31;
		matView._33 = matCamView._33;
	}
	else
	{
		matView = pCam->Camera()->GetViewMat();
	}
	matView = XMMatrixInverse(NULL, matView);

	// 크기
	XMMATRIX matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	matView *= matScale;

	// 이동
	matView._41 = vPos.x; matView._42 = vPos.y; matView._43 = vPos.z;

	Transform()->SetBillBoardMatrix(matView);

	return 0;
}

int CBillBoardScript::DelayUpdate()
{
#ifdef _DEBUG
	// 툴에서 테스트용
	if (!m_bOnce)
	{
		Transform()->SetBillBoard();
		m_bOnce = true;
	}
#endif

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vScale = Transform()->GetLocalScale();

	CGameObject* pCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);

	XMMATRIX matView = pCam->Camera()->GetViewMat();
	matView = XMMatrixInverse(NULL, matView);

	// 크기
	XMMATRIX matScale = XMMatrixScaling(vScale.x, vScale.y, vScale.z);
	matView *= matScale;

	// 이동
	matView._41 = vPos.x; matView._42 = vPos.y; matView._43 = vPos.z;

	Transform()->SetBillBoardMatrix(matView);

	return 0;
}