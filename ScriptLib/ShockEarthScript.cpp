#include "ShockEarthScript.h"

CShockEarthScript::CShockEarthScript()
	:m_fLifeTime(0.f)
	, m_bUp(true)
	, m_bRemain(false)
{
}

CShockEarthScript::~CShockEarthScript()
{
}


int CShockEarthScript::Update()
{
	Vec3 vPos = Transform()->GetLocalPos();

	if (m_bUp)
	{
		if (vPos.y < -20)
			vPos.y += 3000 * DT();

		if (vPos.y >= -20)
			vPos.y = -20;
	}
	
	else if(m_bRemain == false)
	{
		if (vPos.y > -300)
			vPos.y -= 3000 * DT();

		if(vPos.y <= -300)
			GetGameObject()->Destroy();
	}
	

	m_fLifeTime += DT();
	if (m_fLifeTime >= 3)
	{
		m_bUp = false;
	}
	
	Transform()->SetLocalPos(vPos);

	return 0;
}

