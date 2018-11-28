#include "AsuraScript.h"

#include "SceneMgr.h"
#include "ShaderMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "Animation_2D.h"
#include "Animator_3D.h"
#include "Collider_3D.h"
#include "Trail.h"
#include "Audio.h"
#include "Light.h"

#include "HDRFilter.h"

#include "ParentScript\PlayerScript.h"
#include "MgrScript\SkillMgr.h"
#include "MgrScript\EffectMgr.h"
#include "MgrScript\SoundMgr.h"
#include "ThirdCamScript.h"
#include "ShockWave.h"
#include "AuraScript.h"
#include "BillBoardScript.h"
#include "ShockWave.h"
#include "ShockEarthScript.h"


CAsuraScript::CAsuraScript()
	: m_iAttack1_State(0)
	, m_iAttack2_State(0)
	, m_iAttack3_State(0)
	, m_iSpawn_State(0)
	, m_fWalkTime(0.f)
	, m_fFilterRange(0.f)
	, m_iRandInt(0)
	, m_fFadeTime(0.f)
	, m_iVibe(0)
	, m_pAura(NULL)
	, m_fWalkAngle(0.f)
	, m_bOnce(false)
	, m_bOnLight(false)
	, m_fBThreshold(0.f)
	, m_fBScale(0.f)
	, m_fUltraWait(0.f)
	, m_fSunWeight(0.f)
	, m_fAccParticle(0.f)
{
	m_ID = SCRIPT_ASURA;
}

CAsuraScript::~CAsuraScript()
{
}

void CAsuraScript::Start()
{
	CBossScript::Start();
	SetAuraTex();

	// 공격 1
	Animator_3D()->AddAnimation(L"Atk_1-1", 62, 136, 1.3f);
	Animator_3D()->AddAnimation(L"Atk_1-2", 138, 212, 1.3f);

	// 공격 2
	Animator_3D()->AddAnimation(L"Atk_2-1", 940, 1031, 1.f);	// Start
	Animator_3D()->AddAnimation(L"Atk_2-2", 879, 940, 1.f);		// Loop
	Animator_3D()->AddAnimation(L"Atk_2-3", 778, 879, 1.f);		// End

	// 공격 3									
	Animator_3D()->AddAnimation(L"Atk_3-1", 1457, 1613, 1.f);

	// 공격 4									
	Animator_3D()->AddAnimation(L"Atk_4-1", 657, 777, 1.f);

	// 즉사기
	Animator_3D()->AddAnimation(L"UltraAtk01", 2923, 3094, 0.8f);

	Animator_3D()->AddAnimation(L"Spawn", 2711, 2862, 1.f, 0.1f);
	Animator_3D()->AddAnimation(L"Modealram", 1690, 1831);
	Animator_3D()->AddAnimation(L"Dead", 213, 423);

	Animator_3D()->AddAnimation(L"Hit", 425, 486, 1.5f);
	Animator_3D()->AddAnimation(L"Run", 2672, 2710, 1.5f, 0.1f);
	Animator_3D()->AddAnimation(L"Wait", 3155, 3195, 1.f, 0.1f);
	Animator_3D()->AddAnimation(L"Walk", 3196, 3256, 1.2f, 0.1f);

	m_iDeadIdx = 423;
	m_bBurnOut = false;

	//m_eState = BOSS_STATE::SPAWN;
	m_eState = BOSS_STATE::WAIT;
	Transform()->SetLocalPos(m_pPlayerObj->Transform()->GetLocalPos());
	
	m_pCamScript = (CThirdCamScript*)CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->GetComponent<CThirdCamScript>();
	m_pAura = (CAuraScript*)GetGameObject()->FindScript(SCRIPT_AURA);

	Transform()->SetCamCull(false);

	CGameObject* pSword = new CGameObject;
	pSword->SetTag(L"Sword_L");
	pSword->AddComponent<CTransform>(new CTransform);
	pSword->AddComponent<CMeshRender>(new CMeshRender);
	pSword->AddComponent<CCollider>(new CCollider_3D);
	pSword->AddComponent<CTrail>(new CTrail);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Monster")->AddGameObject(pSword);
	Animator_3D()->SetPartsObj(34, pSword);

	pSword->Transform()->SetLocalPos(Vec3(95.f, 210.f, 115.f));
	pSword->Transform()->SetLocalRot(Vec3(0.13f, 0.f, -0.2f));
	pSword->Transform()->SetLocalScale(Vec3(10.f, 150.f, 10.f));
	pSword->Transform()->SetCamCull(false);

	pSword->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pSword->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->MeshRender()->SetPostEffect(true);

	pSword->Collider()->SetColType(COLLIDER_TYPE::OBB);
	pSword->Collider()->SetOffsetPos(Vec3(0.f, -0.2f, 0.f));
	pSword->Collider()->SetOffsetScale(Vec3(3.f, 1.5f, 3.f));

	pSword->Trail()->SetPos(0, Vec3(0.f, 0.5f, 0.f));
	pSword->Trail()->SetPos(1, Vec3(0.f, -0.5f, 0.f));
	pSword->Trail()->SetTrailMtrl(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_02", L"Texture\\Trail\\Trail_02.png"));

	m_pSwordObj[0] = pSword;

	pSword = new CGameObject;
	pSword->SetTag(L"Sword_R");
	pSword->AddComponent<CTransform>(new CTransform);
	pSword->AddComponent<CMeshRender>(new CMeshRender);
	pSword->AddComponent<CCollider>(new CCollider_3D);
	pSword->AddComponent<CTrail>(new CTrail);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Monster")->AddGameObject(pSword);
	Animator_3D()->SetPartsObj(61, pSword);

	pSword->Transform()->SetLocalPos(Vec3(45.f, 210.f, -160.f));
	pSword->Transform()->SetLocalRot(Vec3(-0.35f, -0.5f, 0.f));
	pSword->Transform()->SetLocalScale(Vec3(10.f, 150.f, 10.f));
	pSword->Transform()->SetCamCull(false);

	pSword->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pSword->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->MeshRender()->SetPostEffect(true);

	pSword->Collider()->SetColType(COLLIDER_TYPE::OBB);
	pSword->Collider()->SetOffsetPos(Vec3(0.f, 0.0f, 0.f));
	pSword->Collider()->SetOffsetScale(Vec3(3.f, 1.5f, 3.f));

	pSword->Trail()->SetPos(0, Vec3(0.f, 0.5f, 0.f));
	pSword->Trail()->SetPos(1, Vec3(0.f, -0.5f, 0.f));
	pSword->Trail()->SetTrailMtrl(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pSword->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_02", L"Texture\\Trail\\Trail_02.png"));

	m_pSwordObj[1] = pSword;

	CGameObject* pEye = new CGameObject;
	pEye->SetTag(L"Eye_L");
	pEye->AddComponent<CTransform>(new CTransform);
	pEye->AddComponent<CMeshRender>(new CMeshRender);
	pEye->AddComponent<CTrail>(new CTrail);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Monster")->AddGameObject(pEye);
	Animator_3D()->SetPartsObj(6, pEye);

	pEye->Transform()->SetLocalPos(Vec3(60.f, 151.f, 6.f));
	pEye->Transform()->SetLocalRot(Vec3(1.5f, 0.f, 0.0f));
	pEye->Transform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
	pEye->Transform()->SetCamCull(false);

	pEye->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pEye->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pEye->MeshRender()->SetPostEffect(true);

	pEye->Trail()->SetPos(0, Vec3(0.f, 0.5f, 0.f));
	pEye->Trail()->SetPos(1, Vec3(0.f, -0.5f, 0.f));
	pEye->Trail()->SetMaxCount(20);
	CMaterial* pTrailMtrl = CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail");
	pTrailMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Trail_Alpha"));
	pEye->Trail()->SetTrailMtrl(pTrailMtrl);
	pEye->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_04", L"Texture\\Trail\\Trail_01.png"));

	m_pEyeObj[0] = pEye;

	pEye = new CGameObject;
	pEye->SetTag(L"Eye_R");
	pEye->AddComponent<CTransform>(new CTransform);
	pEye->AddComponent<CMeshRender>(new CMeshRender);
	pEye->AddComponent<CTrail>(new CTrail);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Monster")->AddGameObject(pEye);
	Animator_3D()->SetPartsObj(6, pEye);

	pEye->Transform()->SetLocalPos(Vec3(65.f, 151.f, -6.f));
	pEye->Transform()->SetLocalRot(Vec3(1.5f, 0.f, 0.0f));
	pEye->Transform()->SetLocalScale(Vec3(3.f, 3.f, 3.f));
	pEye->Transform()->SetCamCull(false);

	pEye->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"RectMesh"));
	pEye->MeshRender()->SetMaterial(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
	pEye->MeshRender()->SetPostEffect(true);

	pEye->Trail()->SetPos(0, Vec3(0.f, 0.5f, 0.f));
	pEye->Trail()->SetPos(1, Vec3(0.f, -0.5f, 0.f));
	pEye->Trail()->SetMaxCount(20);
	pTrailMtrl = CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail");
	pTrailMtrl->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_Trail_Alpha"));
	pEye->Trail()->SetTrailMtrl(pTrailMtrl);
	pEye->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_04", L"Texture\\Trail\\Trail_01.png"));

	m_pEyeObj[1] = pEye;

	m_pEyeObj[0]->SetActive(false);
	m_pEyeObj[1]->SetActive(false);
		

	if (nullptr != m_pPlayerObj)
	{
		Vec3 vPos = m_pPlayerObj->Transform()->GetLocalPos() + (m_pPlayerObj->Transform()->GetDir(DIR_TYPE::RIGHT) * 100.f);
		Transform()->SetLocalPos(vPos);
		//vPos = Vec3(9800.00, 1.f, 7700.00);
		m_vStartPos = vPos;
	}
}

int CAsuraScript::Update()
{
	CBossScript::Update();

	if (BOSS_STATE::ATK_01 == m_eState || BOSS_STATE::ATK_03 == m_eState)
	{
		m_pSwordObj[0]->Trail()->SetActive(true);
		m_pSwordObj[1]->Trail()->SetActive(true);
		//SetTrailActive(true);
	}
	else
	{
		m_pSwordObj[0]->Trail()->SetActive(false);
		m_pSwordObj[1]->Trail()->SetActive(false);
		//SetTrailActive(false);
	}
	m_pEyeObj[0]->Trail()->SetActive(true);/*m_pEyeObj[0]->Trail()->SetActive(true);	m_pEyeObj[0]->SetActive(true);
	m_pEyeObj[1]->Trail()->SetActive(true);
	m_pEyeObj[1]->SetActive(true);*/

	return 0;
}

int CAsuraScript::DelayUpdate()
{

	return 0;
}

void CAsuraScript::OnCollisionEnter(CCollider * _pOther)
{
	int iCurAnimIdx = m_pPlayerScript->Animator_3D()->GetCurFrameIdx();

	if (L"Sword" == _pOther->GetGameObject()->GetTag())
	{
		int iCurAnimIdx = m_pPlayerScript->Animator_3D()->GetCurFrameIdx();

		if (PLAYER_STATE::ATK_01 == m_pPlayerScript->GetState() && iCurAnimIdx < 48 && iCurAnimIdx >= 28)
		{
			m_pPlayerObj->Animator_3D()->SetRSSpeed(0.0f, 0.04f);
			GetDamage(20, true);
		}

		else if (PLAYER_STATE::ATK_01_1 == m_pPlayerScript->GetState())
		{
			m_pPlayerObj->Animator_3D()->SetRSSpeed(0.0f, 0.04f);
			GetDamage(20, true);
		}

		if (PLAYER_STATE::ATK_02 == m_pPlayerScript->GetState() && 250 > iCurAnimIdx)
			GetDamage(20, true);

		if (PLAYER_STATE::ATK_04_2 == m_pPlayerScript->GetState())
		{
			GetDamage(1, true);
		}
	}
}

void CAsuraScript::OnCollision(CCollider * _pOther)
{
	if ((_pOther->GetGameObject()->GetLayerName() == L"Default" || _pOther->GetGameObject()->GetLayerName() == L"Rock")
		&&	_pOther->GetGameObject() != m_pPlayerObj && _pOther->GetGameObject()->GetParentObj() !=  m_pPlayerObj)
	{
		Vec3 vColPos = _pOther->GetGameObject()->Transform()->GetLocalPos();
		Vec3 vDist = vColPos - m_vPos;
		
		if (vDist.Distance() != 0)
		{
			vDist.Normalize();
			m_vPos -= vDist * 300.f * DT();
			Transform()->SetLocalPos(m_vPos);
		}
	}
}

void CAsuraScript::ClearEffect()
{
	for (UINT i = 0; i < m_vecEffectObj.size(); ++i)
	{
		m_vecEffectObj[i]->Destroy();
		m_vecEffectObj[i] = NULL;
	}
	m_vecEffectObj.clear();
}

void CAsuraScript::SetAuraTex()
{
	CAuraScript* pAura = (CAuraScript*)GetGameObject()->FindScript(SCRIPT_AURA);
	if (pAura == nullptr)
		return;
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"DarkAura", L"Texture//Effect//DarkElectric.jpg");
	pAura->SetAuraTex(pTex);
}

#include "TileEffect.h"
void CAsuraScript::ShockEarth()
{
	m_fAccTime += DT();

	if (m_fAccTime >= 0.06 && m_iEarthCount <= 5)
	{
		m_fAccTime = 0.f; m_iEarthCount += 1;
		m_vEarthPos = m_vEarthPos + Transform()->GetDir(DIR_TYPE::RIGHT) * 300;	m_vEarthPos.y = -300;
		float fRange = m_iEarthCount * 0.5;

		Vec3 vCrackPos = m_vEarthPos; vCrackPos.y = 2;
		
		for (int i = 0; i < 10; ++i)
		{			
			int iRock = getRandNum_int(1, 4);
			CGameObject* pObj = nullptr;
			switch (iRock)
			{
			case 1: pObj = CResMgr::GetInst()->FindPrefab(L"Rock1.mdat")->Instantiate(); pObj->Transform()->SetLocalScale(Vec3(0.1f, 0.1f, 0.1f) * 4 * fRange); break;
			case 2: pObj = CResMgr::GetInst()->FindPrefab(L"Rock2.mdat")->Instantiate(); pObj->Transform()->SetLocalScale(Vec3(0.1f, 0.1f, 0.1f) * 4 * fRange); break;
			case 3: pObj = CResMgr::GetInst()->FindPrefab(L"Rock3.mdat")->Instantiate(); pObj->Transform()->SetLocalScale(Vec3(0.1f, 0.1f, 0.1f) * 4 * fRange); break;
			case 4: pObj = CResMgr::GetInst()->FindPrefab(L"Rock4.mdat")->Instantiate(); pObj->Transform()->SetLocalScale(Vec3(0.05f, 0.05f, 0.05f) * 4 * fRange); break;
			}
			
			Vec3 vPos = m_vEarthPos;			
			float x = getRandNum_float(-200 * fRange, 200 * fRange); vPos.x += x;
			float y = getRandNum_float(-200 * fRange, 200 * fRange); vPos.z += y;			
			pObj->Transform()->SetLocalPos(vPos);
			
			CShockEarthScript* pScript = new CShockEarthScript;
			pObj->AddComponent<CScript>(pScript);
			CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Rock")->AddGameObject(pObj);

			if (m_iEarthCount == 5 && i == 9)
			{
				pScript->RemainStone(true);
				pObj->AddComponent<CCollider>(new CCollider_3D);
				pObj->Collider()->SetColType(COLLIDER_TYPE::OBB);
				switch (iRock)
				{
				case 1: pObj->Collider()->SetOffsetScale(Vec3(600, 300, 300)); pObj->Collider()->SetOffsetPos(Vec3(0, 200, 0)); break;
				case 2: pObj->Collider()->SetOffsetScale(Vec3(350, 200, 230)); pObj->Collider()->SetOffsetPos(Vec3(0, 100, 0)); break;
				case 3: pObj->Collider()->SetOffsetScale(Vec3(260, 180, 380)); pObj->Collider()->SetOffsetPos(Vec3(0, 100, 0)); break;
				case 4: pObj->Collider()->SetOffsetScale(Vec3(650, 700, 600)); pObj->Collider()->SetOffsetPos(Vec3(50, 350, 0)); break;
				}		
				CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Rock");
				pLayer->AddGameObject(pObj);
			}
			
			for (int i = 0; i < 3; ++i)
			{
				vPos = m_vEarthPos;
				x = getRandNum_float(-200 * fRange, 200 * fRange); vPos.x += x;
				y = getRandNum_float(-200 * fRange, 200 * fRange); vPos.z += y;
				vPos.y = 2;

				pObj = Instantiate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::CRACK_TILE), vPos);
				((CTileEffect*)pObj->GetComponent<CTileEffect>())->SetTile(TILE_TYPE::CRACK);
				pObj->Transform()->SetLocalScale(Vec3(200, 200, 200));
			}
		}
	}
}

#include "SunScript.h"

void CAsuraScript::CreateSun()
{
	CGameObject* pObj = CEffectMgr::GetInst()->FindEffect(L"Sun2")[0];
	Vec3 vPos = m_vPos; vPos.y += 400;
	pObj->Transform()->SetLocalPos(vPos);
	pObj->Transform()->SetLocalScale(Vec3(0.0001, 0.0001, 0.0001));
	
	// 스크립트에 정보입력
	pObj->SetActive(true);
	CSunScript* pScript = (CSunScript*)pObj->FindScript(SCRIPT_ID::SCRIPT_NAVI);
	pScript->SetBoss(GetGameObject());

	// 태양 빛넣기
	if (nullptr == pObj->Light())
	{
		CLight* pLight = new CLight;
		tLightCol tCol = {};
		tCol.vDiff = Vec4(1.f, 0.f, 0.f, 1.f);
		tCol.vSpec = Vec4(1.f, 1.f, 1.f, 1.f);
		tCol.vAmb = Vec4(1.f, 1.f, 1.f, 1.f);

		Vec3 vLightDir = Vec3(1.f, 1.f, 1.f);
		vLightDir.Normalize();
		tLightInfo tInfo(tCol, Vec4(vPos.x, vPos.y, vPos.z, 1.f), vLightDir, (UINT)LIGHT_TYPE::POINT);

		pObj->AddComponent<CLight>(pLight);
		pLight->SetLightInfo(tInfo);
	}	
}

void CAsuraScript::Spawn()
{
	CCamFilter* PFilter = NULL;

	Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
	Vec3 vDist = vPlayerPos - m_vPos;

	float fDist = vDist.Distance();
	vDist.Normalize();
	Vec3 vDir = vDist;

	float fRadian = atan2(-vDir.x, -vDir.z);
	m_vRot.y = fRadian + 1.57f;

	if (0 == m_iSpawn_State)
	{
		Animator_3D()->PlayAnimation(L"Walk");

		m_vPos = Vec3(65000000.f, 5000000000.f, 10000000.f);
		PFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_Fade");

		m_fFadeTime += DT();
		float fDate = 0.f;
		if (m_fFadeTime <= 1.0f)
		{
			CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(PFilter);
			fDate = m_fFadeTime / 1.f;

			if (fDate >= 0.98f)
				fDate = 1.f;

			PFilter->GetMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fDate);

			if (fDate >= 1.f)
			{
				m_pCamScript->SetState(CAMERA_STATE::FREE);

				m_vStartPos.y = 550.f;
				m_pCamScript->Transform()->SetLocalPos(m_vStartPos);

				Vec3 vDir = m_vStartPos - m_pPlayerObj->Transform()->GetLocalPos();
				vDir.y = 0.f;
				vDir.Normalize();
				m_pCamScript->Transform()->SetLocalRot(Vec3(0.f, atan2(vDir.x, vDir.z), 0.f));
			}
		}
		else if(1.5f < m_fFadeTime && m_fFadeTime <= 2.5f)
		{
			fDate = 1 - (m_fFadeTime - 1.5f) / 1.f;
			PFilter->GetMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fDate);
		}
		else if(m_fFadeTime > 2.5f)
		{
			
			m_vStartPos = m_pCamScript->Transform()->GetLocalPos() + (m_pCamScript->Transform()->GetDir(DIR_TYPE::FRONT) * 850.f);
			m_vStartPos -= (m_pCamScript->Transform()->GetDir(DIR_TYPE::RIGHT) * 25.f);
			m_vStartPos.y = 630.f;
			CGameObject* pObj = Instantiate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::POTAL), m_vStartPos);
			pObj->Animator_2D()->PlayAnimation(L"Potal", false);
			pObj->Transform()->SetLocalScale(Vec3(1000.f, 1000.f, 1000.f));
			m_vecEffectObj.push_back(pObj);		

			m_vStartPos = m_pCamScript->Transform()->GetLocalPos() + (m_pCamScript->Transform()->GetDir(DIR_TYPE::FRONT) * 830.f);
			pObj = Instantiate(CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::WAVE), m_vStartPos);
			pObj->AddScript(SCRIPT_BILLBOARD, new CBillBoardScript);
			pObj->Transform()->SetLocalScale(Vec3(750.f, 500.f, 750.f));
			pObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindMesh(L"RectMesh"));
			pObj->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"Mtrl_Potal_Effect"));
			m_vecEffectObj.push_back(pObj);
			pObj->Start();

			m_iSpawn_State = 1;

			m_fFadeTime = 0.f;
		}	
	
	}
	else if (1 == m_iSpawn_State)
	{

		if (0.1f < m_vecEffectObj[0]->Animator_2D()->GetCurAnimPrecent())
		{
			m_vStartPos = m_pCamScript->Transform()->GetLocalPos() + (m_pCamScript->Transform()->GetDir(DIR_TYPE::FRONT) * 800.f);

			m_vPos = m_vStartPos;// Vec3(6500.f, 500.f, 10000.f);
			m_iSpawn_State = 2;	
			Audio()->Play(L"Boss_Walk02");
			m_vStartPos.y = 500.f;
		}
	}
	else if (2 == m_iSpawn_State)
	{
		m_fWalkTime += CTimeMgr::GetInst()->DeltaTime();
		if (false == m_vecEffectObj.empty())
		{
			UINT iFIdx = ((CAnimation_2D*)m_vecEffectObj[0]->Animator_2D()->GetCurAnim())->GetCurFrame();
			if (iFIdx >= 10 & m_fWalkTime < 1.f)
			{
				((CAnimation*)m_vecEffectObj[0]->Animator_2D()->GetCurAnim())->SetFrameIdx(5);
			}


			if (0.5f <= m_vecEffectObj[0]->Animator_2D()->GetCurAnimPrecent())
			{
				ClearEffect();
			}
		}
	}
	else if (3 == m_iSpawn_State)
	{
		PFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_Ripple");

		if (Animator_3D()->GetCurAnimPrecent() > 0.4f && 0 == m_iVibe)
		{					
			//m_pCamScript->SetVibration(true, 400.f);
			m_iVibe = 1;
			CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(PFilter);
			m_pAura->SetActive(true);
		}
		else if(Animator_3D()->GetCurAnimPrecent() >= 0.7f)
		{
			if (1 == m_iVibe)
			{
				//m_pCamScript->SetVibration(false, 0.f, true);
				m_iVibe = 2;
				m_pAura->SetActive(false);
			}
			else
			{
				m_fFilterRange += DT() / 1;
				PFilter->GetMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &m_fFilterRange);

				if (m_fFilterRange > 0.8f && 2 == m_iVibe)
				{
					PFilter = nullptr;
					CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(PFilter);
					m_iVibe = 3;
				}
			}
		}
	}
	else if (4 == m_iSpawn_State)
	{
		Vec3 vRot = m_pCamScript->Transform()->GetLocalRot();
		Vec3 vPos = m_vPos; vPos.z -= 84; vPos.x -= 80; vPos.y += 30;
		if (2767 == m_iCurFrameIdx)
		{
			CEffectMgr::GetInst()->ActiveEffect(L"Dust_01", vPos, Vec3(4800, 4800, 4800), 0.04f, true);
		}

		if (2784 == m_iCurFrameIdx)
		{
			CEffectMgr::GetInst()->ActiveEffect(L"Dust_01", vPos, Vec3(4800, 4800, 4800), 0.04f, true);
		}
		if (2790 == m_iCurFrameIdx)
		{
			CEffectMgr::GetInst()->ActiveEffect(L"Dust_03", Vec3(vPos.x - 20, vPos.y, vPos.z + 120), Vec3(3000, 3000, 3000), 0.04f, true);
			CEffectMgr::GetInst()->ActiveEffect(L"Dust_03", Vec3(vPos.x - 20, vPos.y, vPos.z + 120), Vec3(-3000, -3000, -3000), 0.04f, true);
		}

		if (vRot.x < 0.7f)
		{
			vRot.x += DT() * 0.25f;
			m_pCamScript->Transform()->SetLocalRot(vRot);
		}
	}

	if (3 >= m_iSpawn_State)
	{
		m_fWalkAngle += DT() * 5.2f;
		m_vPos.y = 500.f + sin(m_fWalkAngle) * 8.f;
	}

	if (Animator_3D()->IsAnimFinish())
	{

		if (2 == m_iSpawn_State && m_fWalkTime > 2.f)
		{
			Animator_3D()->PlayAnimation(L"Modealram", true);
			Audio()->Stop();
			Audio()->Play(L"Boss_Modealram", 1);
			Audio()->SetVolume(3.f);
			m_iSpawn_State = 3;
		}
		else if (3 == m_iSpawn_State)
		{
			m_vPos.y = 0.f;
			Animator_3D()->PlayAnimation(L"Spawn", true);
			Audio()->Stop();
			Audio()->SetVolume(1.f);
			Audio()->Play(L"Boss_Spawn", 1);
			m_iSpawn_State = 4;
		}
		else if (4 == m_iSpawn_State)
		{
			m_fWalkTime = 0.f;
			m_eState = BOSS_STATE::WALK;
			Animator_3D()->PlayAnimation(L"Walk");
			m_iSpawn_State = 0;
			m_fFilterRange = 0.f;
			m_iVibe = 0;
			m_pCamScript->SetState(CAMERA_STATE::FOLLOW);
			m_pCamScript->SetTargetObj(m_pPlayerObj);
			m_pHPBar->SetActive(true);
			m_fWalkAngle = 0;
		}
	}
	m_iCurFrameIdx; 

}

void CAsuraScript::Wait()
{
	CBossScript::Wait();

	if (nullptr == m_pPlayerObj)
		return;

	Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
	Vec3 vDist = vPlayerPos - m_vPos;

	float fDist = vDist.Distance();

	m_fWaitTime += DT();

	if (m_fWaitTime >= 0.5f || m_fUltraWait > 0.f)
	{
		m_fWaitTime = 0;
	
		CLayer* pLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Rock");
		
		float fPer = m_iHP / m_iMaxHP;

		if (1.f <= fPer && 5 <= pLayer->GetLayerObjSize())
		{
			
			m_fUltraWait += DT();
		}
		else if(0.5f > fPer && 3 <= pLayer->GetLayerObjSize())
		{
			m_fUltraWait += DT();
		}
		else
		{			
			if (fDist <= m_fDist_Att)
			{
				m_iRandInt = getRandNum_int(0, 99);

				if (m_iRandInt < 50)
					m_eState = BOSS_STATE::ATK_01;
				else if (50 <= m_iRandInt && m_iRandInt < 75)
					m_eState = BOSS_STATE::ATK_02;
				else if (75 <= m_iRandInt && m_iRandInt < 100)
					m_eState = BOSS_STATE::ATK_03;
			}
			else
			{
				m_eState = BOSS_STATE::WALK;
				Animator_3D()->PlayAnimation(L"Walk");
				m_fSunWeight = 0.f;
			}
		}

		if (m_fUltraWait >= 3.f)
		{
			m_fUltraWait = 0;
			m_eState = BOSS_STATE::ATK_ULTRA;
		}
	}
}

void CAsuraScript::Walk()
{
	CBossScript::Walk();

	if (3196 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		Audio()->Play(L"Boss_Walk01", 1);
	}

	m_fSunWeight += DT() * 10.f;

	if (m_fSunWeight > 100)
	{
		m_eState = BOSS_STATE::ATK_04;
		m_fSunWeight = 0.f;
	}
}

void CAsuraScript::Atk_01()
{
	if (0 == m_iAttack1_State)
	{
		Animator_3D()->PlayAnimation(L"Atk_1-1", true);
		m_pSwordObj[0]->SetActive(false);
		m_pSwordObj[1]->SetActive(true);
	}
	else if (1 == m_iAttack1_State)
	{
		Animator_3D()->PlayAnimation(L"Atk_1-2", true);
		m_pSwordObj[0]->SetActive(true);
		m_pSwordObj[1]->SetActive(false);
	}

	if (m_iCurFrameIdx == 62 || m_iCurFrameIdx == 138)
	{
		Audio()->Stop();
		Audio()->Play(L"Boss_Atk01", 1);
	}
	else if (m_iCurFrameIdx == 135 || m_iCurFrameIdx == 211)
	{
		if (0 == m_iAttack1_State)
		{
			m_iAttack1_State = 1;
			m_pSwordObj[0]->SetActive(true);
		}
		else if (1 == m_iAttack1_State)
		{
			m_iAttack1_State = 0;
			m_pSwordObj[1]->SetActive(true);
		}

		SetTrailActive(false);

		m_eState = BOSS_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 50;		
	}
}

void CAsuraScript::Atk_02()
{
	if (0 == m_iAttack2_State)
	{
		Animator_3D()->PlayAnimation(L"Atk_2-1", true);	

		if (940 == Animator_3D()->GetCurFrameIdx())
		{
			Audio()->Stop();
			Audio()->Play(L"Boss_LongAtk", 1);
		}
	}
	if (1 == m_iAttack2_State)
	{
		Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
		Vec3 vDist = vPlayerPos - m_vPos;

		float fDist = vDist.Distance();
		vDist.Normalize();
		Vec3 vDir = vDist;

		Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);
		Vec3 vCross = vFront.Cross(vDir);
		float fRotY = vFront.Dot(vDir);

		if (vCross.y > 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y += DT();
		}

		else if (vCross.y < 0)
		{
			if (fRotY <= 0.99)
				m_vRot.y -= DT();
		}
	}

	else if (2 == m_iAttack2_State)
	{
		if (Animator_3D()->GetCurAnimPrecent() >= 0.35f)
		{
			Vec3 vPos = Transform()->GetLocalPos();
			CShockWave* pScript = (CShockWave*)CSkillMgr::GetInst()->UseSkill(SKILL_TYPE::SHOCKWAVE, vPos, GetGameObject());

			vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 120.f;
			pScript->Init(vPos);
			m_iAttack2_State = 3;
			m_pAura->SetActive(false);
			m_vEarthPos = m_vPos;
			m_fAccTime = 0.3; m_iEarthCount = 0;
			ShockEarth();
		}
	}
	else if (3 == m_iAttack2_State)
	{
		ShockEarth();
	}

	if (Animator_3D()->IsAnimFinish() || Animator_3D()->GetCurAnimPrecent() >= 0.9f)
	{
		if (0 == m_iAttack2_State)
		{
			m_iAttack2_State = 1;
			m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 25.f;
			Animator_3D()->PlayAnimation(L"Atk_2-2", true);
			m_pAura->SetActive(true);

			Audio()->Stop();
			Audio()->Play(L"Boss_HeavyAtk", 1);
		}
		else if (1 == m_iAttack2_State)
		{
			m_iAttack2_State = 2;
			Animator_3D()->PlayAnimation(L"Atk_2-3", true);
		}
		else if (3 == m_iAttack2_State)
		{
			m_iAttack2_State = 0;

			m_eState = BOSS_STATE::WAIT;
			Animator_3D()->PlayAnimation(L"Wait");
		}
	}
}

void CAsuraScript::Atk_03()
{
	Animator_3D()->PlayAnimation(L"Atk_3-1", true);

	if (1457 == m_iCurFrameIdx)
	{
		Audio()->Stop();
		Audio()->Play(L"Boss_LongAtk", 1);
	}

	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = BOSS_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");

		m_iAttack3_State = 0;
	}
	else if (0 == m_iAttack3_State && Animator_3D()->GetCurAnimPrecent() > 0.5f)
	{
		Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
		Vec3 vDist = vPlayerPos - m_vPos;

		vDist.Normalize();
		Vec3 vDir = vDist;

		m_vPos += vDir * m_fSpeed * DT() * 100.f;

		m_iAttack3_State = 1;
	}
	else if (1 == m_iAttack3_State && Animator_3D()->GetCurAnimPrecent() > 0.8f)
	{
		m_iAttack3_State = 2;
		ClearEffect();
	}
}

void CAsuraScript::Atk_04()
{
	Animator_3D()->PlayAnimation(L"Atk_4-1", true);

	m_pAura->SetActive(true);
	Vec3 vPlayerPos = m_pPlayerObj->Transform()->GetLocalPos();
	Vec3 vDist = vPlayerPos - m_vPos;

	float fDist = vDist.Distance();
	vDist.Normalize();
	Vec3 vDir = vDist;

	Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);
	Vec3 vCross = vFront.Cross(vDir);
	float fRotY = vFront.Dot(vDir);

	if (vCross.y > 0)
	{
		if (fRotY <= 0.99)
			m_vRot.y += DT() * 1.5;
	}

	else if (vCross.y < 0)
	{
		if (fRotY <= 0.99)
			m_vRot.y -= DT() * 1.5;
	}

	if (696 == m_iCurFrameIdx && m_bOnce == false)
	{
		Vec3 vPos = m_vPos; vPos.y += 450;

		CreateSun();
		CEffectMgr::GetInst()->ActiveEffect(L"LightBoom_2", vPos, Vec3(2250, 1500, 2250), 0.03);
		m_bOnce = true;
	}
	
	if (Animator_3D()->IsAnimFinish())
	{
		m_bOnce = false;
		m_eState = BOSS_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
	
		m_pAura->SetActive(false);
	}
}

void CAsuraScript::Atk_Ultra()
{
	Animator_3D()->PlayAnimation(L"UltraAtk01");

	static bool g_bOnce = false;

	SplatParticle();
	int iStartIdx = 2922;

	if (iStartIdx + 25 == m_iCurFrameIdx && false == g_bOnce)			// 준비
	{
		// 카메라 암전
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetAllLayerUnCheck();
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetSkyboxRender(false);
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->LayerRenderCheck(L"Monster");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->LayerRenderCheck(L"Effect");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->LayerRenderCheck(L"Player");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->LayerRenderCheck(L"Rock");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::UI)->SetActive(false);

		Transform()->SetRimNum(2);
		m_pAura->SetActive(true);
		g_bOnce = true;

		CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::FLARE_1);
		CGameObject* pObj = Instantiate(pPrefab, m_vPos, L"Effect");
		pObj->Animator_2D()->PlayAnimationReverse(L"Flare_1");
		m_vecEffectObj.push_back(pObj);

		Audio()->Stop();
		Audio()->Play(L"Boss_UltraAtk", 1);
		
	}
	else if (iStartIdx + 60 < m_iCurFrameIdx && m_iCurFrameIdx <= 2986
		&& m_vecEffectObj.empty())
	{
		CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::FLARE_2);
		CGameObject* pObj = Instantiate(pPrefab, m_vPos, L"Effect");
		pObj->Animator_2D()->PlayAnimationReverse(L"Flare_2");
		m_vecEffectObj.push_back(pObj);
	}
	else if (iStartIdx + 115 == m_iCurFrameIdx && false == m_bOnLight)	// 시전
	{
		Transform()->SetRimNum(3);
		m_fBThreshold = 0.1f;
		m_fBScale = 3.f;
		((CHDRFilter*)CRenderMgr::GetInst()->FindFilter(L"Filter_HDR"))->SetBloomValue(m_fBThreshold, m_fBScale);
		m_pPlayerScript->CheckHide(Transform()->GetLocalPos());
		m_bOnLight = true;
		m_pCamScript->SetVibration(true, 300.f);

		CPrefab* pPrefab = CEffectMgr::GetInst()->GetEffect(EFFECT_TYPE::FLARE_0);
		Vec3 vPos = m_vPos;
		vPos.y += 500.f;
		CGameObject* pObj = Instantiate(pPrefab, vPos, L"Effect");
		pObj->Animator_2D()->PlayAnimation(L"Flare_0", false);
		m_vecEffectObj.push_back(pObj);
	}
	else if (iStartIdx + 125 == m_iCurFrameIdx)
	{
		m_pCamScript->SetVibration(false);
		m_pAura->SetActive(false);
	}
	else if (iStartIdx + 139 == m_iCurFrameIdx)
	{
		Transform()->SetRimNum(2);
	}
	else if (iStartIdx + 150 == m_iCurFrameIdx)
	{
		Transform()->SetRimNum(0);
		((CHDRFilter*)CRenderMgr::GetInst()->FindFilter(L"Filter_HDR"))->SetDefault();
		m_bOnLight = false;
		m_fBThreshold = 0.f;
		m_fBScale = 0.f;
		m_eState = BOSS_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->RevertLayer();
		CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::UI)->SetActive(true);
		g_bOnce = false;
		ClearEffect();
	}

	if (m_bOnLight)
	{
		m_fBThreshold += DT() * 2.f;
		m_fBScale -= DT() * 1.7f;
		((CHDRFilter*)CRenderMgr::GetInst()->FindFilter(L"Filter_HDR"))->SetBloomValue(m_fBThreshold, m_fBScale);
	}

	if (iStartIdx + 60 < m_iCurFrameIdx &&
		false == m_vecEffectObj.empty())
	{
		CGameObject* pObj = m_vecEffectObj.front();
		if (pObj->Animator_2D()->IsAnimFinish())
		{
			pObj->Destroy();
			m_vecEffectObj.clear();
		}
	}
}


void CAsuraScript::SplatParticle()
{
	int iStartIdx = 2922;
	if (iStartIdx + 25 <= m_iCurFrameIdx && m_iCurFrameIdx <= iStartIdx + 110)
	{
		m_fAccParticle += DT();
		if (m_fAccParticle >= 0.2)
		{
			for (int i = 0; i < 3; ++i)
			{
				Vec3 vPos = m_vPos;
				vPos.x += getRandNum_float(-800, 800);	vPos.y += getRandNum_float(0, 800);	vPos.z += getRandNum_float(-800, 800);
				CEffectMgr::GetInst()->ActiveEffect(L"Particle_Electric_01", vPos, Vec3(800, 800, 800), 0.05);
				vPos = m_vPos;
				vPos.x += getRandNum_float(-800, 800);	vPos.y += getRandNum_float(0, 800);	vPos.z += getRandNum_float(-800, 800);
				CEffectMgr::GetInst()->ActiveEffect(L"Particle_Electric_02", vPos, Vec3(800, 800, 800), 0.05);
				vPos = m_vPos;
				vPos.x += getRandNum_float(-800, 800);	vPos.y += getRandNum_float(0, 800);	vPos.z += getRandNum_float(-800, 800);
				CEffectMgr::GetInst()->ActiveEffect(L"Particle_Electric_03", vPos, Vec3(800, 800, 800), 0.05);
				m_fAccParticle = 0.f;
			}
		}
	}
}

void CAsuraScript::Dead()
{
	CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_Fade");
	CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);
	m_fFadeTime += DT();
	pFilter->GetMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &m_fFadeTime);
}