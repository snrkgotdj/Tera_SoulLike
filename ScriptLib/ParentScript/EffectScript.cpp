#include "EffectScript.h"

CEffectScript::CEffectScript()
	: m_bStart(false)
	, m_fLifeTime(0.f)
{
}

CEffectScript::~CEffectScript()
{
}

int CEffectScript::Update()
{
	m_fLifeTime += DT();
	if (m_fLifeTime > 30.f)
		GetGameObject()->Destroy();

	return 0;
}