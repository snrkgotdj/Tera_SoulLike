#include "EnergyBall.h"

#include "MgrScript\EffectMgr.h"
#include "ParentScript\PlayerScript.h"
#include "ElectricEffect.h"

#include "Animator_3D.h"

CEnergyBall::CEnergyBall()
	: m_bStart(false)
	, m_bEnd(false)
{
	m_ID = SCRIPT_ENERGYBALL_SKILL;
}

CEnergyBall::~CEnergyBall()
{
}

int CEnergyBall::Update()
{
	if (m_pObj->IsDestroy())
	{
		if (NULL != m_pCastObj)
			m_pCastObj->Destroy();

		if (false == m_bStart)
		{
			GetGameObject()->Destroy();
		}
		return 0;
	}

	if (false == m_bStart)
	{
		Vec3 vScale = Transform()->GetLocalScale();
		
		if (vScale.x < 15.f)
		{
			vScale += DT() * 30.f;
			Transform()->SetLocalScale(vScale);
		}

		Vec3 vCastScale = m_pCastObj->Transform()->GetLocalScale();
		if (vCastScale.x > 2.f)
		{
			vCastScale -= DT() * 30.f;
			m_pCastObj->Transform()->SetLocalScale(vCastScale);
		}
	}
	else
	{
		if (m_bEnd)
		{
			Vec3 vScale = Transform()->GetLocalScale();

			// 맞았을때
			if (m_pCastObj->Animator_2D()->IsAnimFinish())
			{
				CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Electric");
				m_vecEffect.front()->GetGameObject()->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_1, &pTex);
				GetGameObject()->Destroy();
				m_pCastObj->Destroy();
			}
			else if(vScale.x < 30.f)
			{
				vScale += DT() * 80.f;
				Transform()->SetLocalScale(vScale);
			}
		}
		else
		{
			Vec3 vPos = Transform()->GetLocalPos();
			vPos += m_vDir * 1000.f * DT();
			Transform()->SetLocalPos(vPos);
		}
	}

	EffectUpdate();

	return 0;
}

void CEnergyBall::OnCollisionEnter(CCollider * _pOther)
{
	if (NULL != m_pCastObj)
		return;

	if (CSceneMgr::GetInst()->GetPlayerObj() == _pOther->GetGameObject())
	{
		CPlayerScript* pScript = (CPlayerScript*)_pOther->GetGameObject()->GetComponent<CPlayerScript>();
		pScript->GetDamage(0);
		m_bEnd = true;

		CTexture* pTex = NULL;
		m_vecEffect.front()->GetGameObject()->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_1, &pTex);

		m_pCastObj = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::ELECTRIC_END), Transform()->GetLocalPos());
		m_pCastObj->Animator_2D()->PlayAnimation(L"ElectricBoom", false);
		Vec3 vPos = m_pCastObj->Transform()->GetLocalPos();
		vPos += m_vDir * 20.f;
		m_pCastObj->Transform()->SetLocalPos(vPos);
		Transform()->SetLocalPos(vPos);
	}
}

void CEnergyBall::CastSkill()
{
	if (NULL != m_pCastObj)
		assert(NULL);

	m_pCastObj = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::ELECTRIC_CAST), m_pObj->Transform()->GetLocalPos());
	m_pCastObj->Animator_2D()->PlayAnimation(L"ElectricCast", false);

	CEffectScript* pScript = EffectCreate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::ELECTRIC), SCRIPT_ID::SCRIPT_ELECTRIC_EFFECT);
	m_vecEffect.push_back(pScript);

	Vec3 vPos = m_pObj->Transform()->GetLocalPos();
	Vec3 vTargetPos = CSceneMgr::GetInst()->GetPlayerObj()->Transform()->GetLocalPos();
	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	vPos.y = 50.f;
	vPos += vDir * 80.f;
	m_pCastObj->Transform()->SetLocalPos(vPos);
	Transform()->SetLocalPos(vPos);
}

void CEnergyBall::SkillStart()
{
	m_bStart = true;
	m_pCastObj->Destroy();
	m_pCastObj = NULL;
}