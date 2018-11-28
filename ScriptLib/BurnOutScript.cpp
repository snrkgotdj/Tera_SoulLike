#include "BurnOutScript.h"
#include "KeyMgr.h"

CBurnOutScript::CBurnOutScript()
	:m_fAccTime(0.f)
	, m_bBurnOut(false)
	, m_fBurnTime(0.7f)
{
	m_ID = SCRIPT_BURNOUT;

}

CBurnOutScript::~CBurnOutScript()
{
}

int CBurnOutScript::Update()
{
	m_fAccTime += DT() * 0.7f;
	
	if (m_fAccTime >= 1)
	{
		m_fAccTime = 1;
		GetGameObject()->Destroy();
	}

	for (int i = 0; i < MeshRender()->GetMaterialSize(); ++i)
	{
		MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::SP_FLOAT_0, &m_fAccTime);
	}

	return 0;
}

