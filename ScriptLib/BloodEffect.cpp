#include "BloodEffect.h"

#include "SceneMgr.h"
#include "GameObject.h"
#include "Terrain.h"

#include "MgrScript\EffectMgr.h"
#include "TileEffect.h"

CBloodEffect::CBloodEffect()
	: m_bSetBlood(false)
{
	m_ID = SCRIPT_Blood_EFFECT;
}

CBloodEffect::~CBloodEffect()
{
}

void CBloodEffect::Start()
{
	Animator_2D()->PlayAnimation(L"Blood", false);
}

int CBloodEffect::Update()
{
	if (Animator_2D()->GetCurAnimPrecent() > 0.4f && false == m_bSetBlood)
	{
		Vec3 vPos = Transform()->GetLocalPos();
		vPos.y = 2.f;
		CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::BLOOD_TILE);
		CGameObject* pObj = Instantiate(pPrefab, vPos, L"Effect");
		pObj->MeshRender()->GetMaterial();
		pObj->Transform()->SetLocalRot(Vec3(1.57f, 0.f, 0.f));
		((CTileEffect*)pObj->GetComponent<CTileEffect>())->SetTile(TILE_TYPE::BLOOD);

		m_bSetBlood = true;
	}

	if (Animator_2D()->IsAnimFinish())
	{
		GetGameObject()->Destroy();
	}

	return 0;
}