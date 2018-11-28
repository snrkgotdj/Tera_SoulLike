#include "UIMgrScript.h"
#include "../ButtonScript.h"

#include "KeyMgr.h"
#include "SceneMgr.h"
#include "Scene.h"

CUIMgrScript::CUIMgrScript()
{
}

CUIMgrScript::~CUIMgrScript()
{
}

int CUIMgrScript::Update()
{
	if (!CSceneMgr::GetInst()->IsPlayMode())
		return RET_FAILED;

	ButtonUpdate();

	return 0;
}

int CUIMgrScript::ButtonUpdate()
{
	if (m_listButton.empty())
		return RET_FAILED;

	CButtonScript* pCurButton = NULL;

	Vec2 vWindowPos = CKeyMgr::GetInst()->GetMousePos();
	Vec2 vResolution = CRenderMgr::GetInst()->GetWindowResolution();

	CGameObject* pCam = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::UI);
	Vec2 vWorldPos = pCam->Camera()->Get2DWorldPosFromWindowPos(vWindowPos, vResolution);

	list<CButtonScript*>::iterator iter = m_listButton.begin();
	for (; iter != m_listButton.end(); ++iter)
	{
		if ((*iter)->IsMouseOn(vWorldPos))
		{
			pCurButton = (*iter);
			(*iter)->SetState(BUTTON_STATE::HIGHLIGHTED);
		}
		else
		{
			(*iter)->SetState(BUTTON_STATE::NONE);
		}
	}

	if (NULL == pCurButton)
		return RET_FAILED;

	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LBTN, KEY_STATE::STATE_TAB))
	{
		pCurButton->OnClick();
		pCurButton->SetState(BUTTON_STATE::CLICK);
	}

	return RET_SUCCESS;
}

void CUIMgrScript::AddButton(CButtonScript * _pButton)
{
	m_listButton.push_back(_pButton);
}