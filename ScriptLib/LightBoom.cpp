#include "LightBoom.h"


CLightBoom::CLightBoom()
{
	m_ID = SCRIPT_LIGHTBOOM;
}

CLightBoom::~CLightBoom()
{
}

int CLightBoom::Update()
{
	Vec3 vScale = Transform()->GetLocalScale();
	Vec3 vPos = Transform()->GetLocalPos();

	vPos.y += 120 * DT();
	vScale.x += 2000 * DT() * 1.5f;
	vScale.y += 2000 * DT()* 1.5f;
	vScale.z += 2000 * DT();

	Transform()->SetLocalScale(vScale);
	Transform()->SetLocalPos(vPos);
	return 0;
}

