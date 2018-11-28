#include "DefaultEffect.h"
#include "Audio.h"
#include "Animation_2D.h"

CDefaultEffect::CDefaultEffect()
	: m_fAccTime(0.f)
{
	m_ID = SCRIPT_DEFAULTEFFECT;
}

CDefaultEffect::~CDefaultEffect()
{
}

void CDefaultEffect::Awake()
{
	GetGameObject()->SetActive(false);
	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));
}

int CDefaultEffect::Update()
{
	if (((CAnimation_2D*)(Animator_2D()->GetCurAnim()))->GetFrameSize() == 1)
	{
		m_fAccTime += DT();
		if (m_fAccTime >= 3)
		{
			m_fAccTime = 0;
			GetGameObject()->SetActive(false);
		}
	}

	else if (Animator_2D()->IsAnimFinish())
	{
		GetGameObject()->SetActive(false);
	}

	return 0;
}
