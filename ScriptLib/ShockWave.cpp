#include "ShockWave.h"

#include "Audio.h"

#include "MgrScript\EffectMgr.h"
#include "ParentScript\PlayerScript.h"
#include "ThirdCamScript.h"
#include "TileEffect.h"

CShockWave::CShockWave()
	: m_pEffectObj1(NULL)
	, m_pEffectObj2(NULL)
	, m_pCamScript(NULL)
	, m_fTime(0.f)
{
	m_ID = SCRIPT_SHOCKWAVE_SKILL;
}

CShockWave::~CShockWave()
{
}

int CShockWave::Update()
{
	if (NULL != m_pEffectObj1)
	{
		Vec3 vScale = m_pEffectObj1->Transform()->GetLocalScale();

		if (vScale.x < 1500.f)
		{
			vScale += 4000.f * DT();
			m_pEffectObj1->Transform()->SetLocalScale(vScale);
			if(nullptr != m_pCamScript)
				m_pCamScript->SetVibration(true, 300.f);
			m_pEffectObj2->Transform()->SetLocalScale(vScale * 4);
		}
		else
		{
			m_pEffectObj1->Destroy();
			m_pEffectObj1 = NULL;
		}
	}

	m_fTime += DT();

	if (m_fTime > 1.5f)
	{
		if(nullptr != m_pCamScript)
			m_pCamScript->SetVibration(false);
		GetGameObject()->Destroy();

		if (nullptr != m_pEffectObj2)
		{
			m_pEffectObj2->Destroy();
			m_pEffectObj2 = NULL;
		}
	}

	return 0;
}

void CShockWave::OnCollisionEnter(CCollider * _pOther)
{
	CPlayerScript* pPlayer = (CPlayerScript*)_pOther->GetGameObject()->GetComponent<CPlayerScript>();

	if (NULL != pPlayer)
		pPlayer->GetDamage(0);
}

void CShockWave::Init(Vec3 _vEffectPos, bool _bIsRock)
{
	m_pEffectObj1 = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::WAVE), _vEffectPos);

	_vEffectPos.y += 250.f;
	m_pEffectObj2 = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::SMOKE1), _vEffectPos);
	m_pEffectObj2->Animator_2D()->PlayAnimation(L"Smoke1", false);
	//m_pEffectObj2->Transform()->SetLocalScale(Vec3(800.f, 800.f, 800.f));

	_vEffectPos.y = 2.f;
	CGameObject* pObj = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::CRACK_TILE), _vEffectPos);
	Vec3 vPos = pObj->Transform()->GetLocalPos();
	pObj->Transform()->SetLocalPos(vPos);
	
	if (_bIsRock)
	{
		((CTileEffect*)pObj->GetComponent<CTileEffect>())->SetTile(TILE_TYPE::CRACK2);
		pObj->Transform()->SetLocalScale(Vec3(1920.f * 0.8f, 1080.f * 0.8f, 1000.f));
	}
	else
	{
		((CTileEffect*)pObj->GetComponent<CTileEffect>())->SetTile(TILE_TYPE::CRACK);
	}

	Transform()->SetLocalScale(Vec3(400.f, 400.f, 400.f));

	m_pCamScript = (CThirdCamScript*)CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->GetComponent<CThirdCamScript>();


	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));
	Audio()->Play(L"Explosion", 1);
	Audio()->SetVolume(10.f);
}
