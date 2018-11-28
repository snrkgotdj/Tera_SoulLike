#include "TestScript.h"

#include "MgrScript\UIMgrScript.h"
#include "ButtonScript.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"

#include "KeyMgr.h"

CTestScript::CTestScript()
	: m_pPlayerObj(NULL)
	, m_fSpeed(100)
{
}

CTestScript::~CTestScript()
{

}

void CTestScript::Awake()
{
	m_pPlayerObj = CSceneMgr::GetInst()->GetPlayerObj();
}

int CTestScript::Update()
{
	m_vPos = Transform()->GetLocalPos();
	CalculateDir();

	m_vPos += m_vDir * m_fSpeed * DT();

	Transform()->SetLocalPos(m_vPos);
	return 0;
}

int CTestScript::DelayUpdate()
{
	return 0;
}

void CTestScript::Move()
{
	
}

void CTestScript::CalculateDir()
{
	m_vDir = m_pPlayerObj->Transform()->GetLocalPos() - m_vPos;
	m_vDir.Normalize();
}
