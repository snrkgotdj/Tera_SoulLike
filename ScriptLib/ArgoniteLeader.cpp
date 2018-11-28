#include "ArgoniteLeader.h"

#include "ResMgr.h"
#include "Mesh.h"

#include "SceneMgr.h"

#include "Scene.h"
#include "Layer.h"

#include "Transform.h"
#include "Animator_3D.h"
#include "Trail.h"
#include "Audio.h"

#include "BurnOutScript.h"
#include "AuraScript.h"
#include "ThirdCamScript.h"

#include "ParentScript\PlayerScript.h"
#include "MgrScript\EffectMgr.h"
#include "MgrScript\SoundMgr.h"

CArgoniteLeader::CArgoniteLeader()
	: m_fTest(44)
	, m_bOnce(false)
	, m_bAuraOn(false)
	, m_pSwordObj(NULL)
{
	m_ID = SCRIPT_ARGONITELEADER;
	//m_eState = PLAYER_STATE::END;
}

CArgoniteLeader::~CArgoniteLeader()
{
}

void CArgoniteLeader::Start()
{
	CAuraScript* pAura = new CAuraScript;
	GetGameObject()->AddComponent<CScript>(pAura);
	pAura->Awake();

	CPlayerScript::Start();

	SetTrailActive(false);
	SetAura(false);
	SetTrailTex();
	SetAuraTex();

	Animator_3D()->AddAnimation(L"Atk_01", 24, 40, 3.f, 0.2f);
	Animator_3D()->AddAnimation(L"Atk_01_1", 1285, 1324, 6.f);
	Animator_3D()->AddAnimation(L"Atk_02", 61, 331, 2.3f, 0.2f);
	Animator_3D()->AddAnimation(L"Atk_03", 332, 422, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Atk_04_1", 423, 484, 3.f);
	Animator_3D()->AddAnimation(L"Atk_04_2", 487, 545, 3.f);
	Animator_3D()->AddAnimation(L"Atk_04_3", 548, 635, 2.f);
	Animator_3D()->AddAnimation(L"Command", 636, 711, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Command01", 712, 772, 1.f, 0.2f);
	Animator_3D()->AddAnimation(L"Dead", 773, 906, 1.f);
	Animator_3D()->AddAnimation(L"DeathWait", 907, 907, 1.f);
	Animator_3D()->AddAnimation(L"DownEnd", 908, 948, 1.f);
	Animator_3D()->AddAnimation(L"DownLoop", 949, 979, 1.f);
	Animator_3D()->AddAnimation(L"DownStart", 980, 1010, 1.f);
	Animator_3D()->AddAnimation(L"DyingWait", 1011, 1071, 1.f);
	Animator_3D()->AddAnimation(L"Hit", 1072, 1117, 1.f);
	Animator_3D()->AddAnimation(L"KnockBackEnd", 1118, 1178, 1.f);
	Animator_3D()->AddAnimation(L"KnockBackLoop", 1179, 1209, 1.f);
	Animator_3D()->AddAnimation(L"KnockBackStart", 1210, 1263, 1.f);
	Animator_3D()->AddAnimation(L"ReadyAction", 1264, 1324, 1.5f);
	Animator_3D()->AddAnimation(L"Run", 1325, 1355, 1.f);
	Animator_3D()->AddAnimation(L"SideWalk01", 1356, 1396, 1.f);
	Animator_3D()->AddAnimation(L"SideWalk02", 1397, 1437, 1.f);
	Animator_3D()->AddAnimation(L"Stun", 1438, 1476, 1.f);
	Animator_3D()->AddAnimation(L"UnarmedWait", 1477, 1557, 1.f);
	Animator_3D()->AddAnimation(L"Wait", 1558, 1618, 1.5f, 0.2f);
	Animator_3D()->AddAnimation(L"Walk", 1619, 1679, 1.5f, 0.2f);

	list<CGameObject*> listChild = GetGameObject()->GetChildList();
	list<CGameObject*>::iterator iter = listChild.begin();
	for (; iter != listChild.end(); ++iter)
	{
		if ((*iter)->Trail())
		{
			m_pSwordObj = (*iter);
			m_pSwordObj->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_3D));
		}
	}
}

void CArgoniteLeader::Atk_01()
{
	if (1 == m_iCurAnimIdx)
	{
		Audio()->Stop();
		if (getRandNum_int(0, 1))
		{
			Audio()->Play(L"Player_Atk01_1", 1);
		}
		else
		{
			Audio()->Play(L"Player_Atk01_2", 1);
		}
	}
	else if (25 == m_iCurAnimIdx)
	{
		m_pSwordObj->Audio()->SetVolume(3.f);
		m_pSwordObj->Audio()->Play(L"Sword_Swing", 2);
		
	}
	else if (28 == m_iCurAnimIdx)
	{
		SetTrailActive(true);
	}
	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::ATK_01_1;
		Animator_3D()->PlayAnimation(L"Atk_01_1");
		
	}
}

void CArgoniteLeader::Atk_01_1()
{
	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		SetTrailActive(false);
	}
}

void CArgoniteLeader::Atk_02()
{
	int iCurFrameIdx = Animator_3D()->GetCurFrameIdx();
	if (121 == iCurFrameIdx && m_bOnce == false)
	{
		Animator_3D()->SetBlur(true);
		CreateAfterImage();
		m_bOnce = true;
	}
	else if (151 == iCurFrameIdx && m_bOnce == true)
	{
		CreateAfterImage();
		m_bOnce = false;
	}
	else if (181 == iCurFrameIdx && m_bOnce == false)
	{
		CreateAfterImage();
		m_bOnce = true;
	}
	else if (208 == iCurFrameIdx && m_bOnce == true)
	{
		CreateAfterImage();
		m_bOnce = false;
	}
	else if (236 == iCurFrameIdx && m_bOnce == false)
	{
		CreateAfterImage();
		m_bOnce = true;
	}
	else if (240 == iCurFrameIdx)
	{
		m_bOnce = false;
	}

	if (true == Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		Animator_3D()->SetBlur(false);
		SetTrailActive(false);
		m_bSuperArmor = false;
	}
	else
	{
		MoveLonger(iCurFrameIdx);
		m_bSuperArmor = true;
	}

	if(m_iCurAnimIdx > 121 && m_iCurAnimIdx < 236)
		SplatParticle();


}

void CArgoniteLeader::Atk_03()
{
	int iCurFrameIdx = Animator_3D()->GetCurFrameIdx();

	if (iCurFrameIdx == 370 && m_bOnce == false)
	{
		Vec3 vEffectPos = m_vPos + Transform()->GetDir(DIR_TYPE::RIGHT) * 100;
		vEffectPos.y += 30;
		//CEffectMgr::GetInst()->ActiveEffect(L"BlueFire_01", vEffectPos, Vec3(60, 60, 60), 0.03f);
		m_bOnce = true;
	}

	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		Animator_3D()->SetBlur(false);
		m_bOnce = false;
	}
}

void CArgoniteLeader::Atk_04_1()
{
	if (Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::ATK_04_2;
		Animator_3D()->PlayAnimation(L"Atk_04_2");
		m_vPos = Transform()->GetLocalPos();
		Animator_3D()->SetBlur(true);
		SetTrailActive(true);
	}
}

void CArgoniteLeader::Atk_04_2()
{
	m_bSuperArmor = true;
	if (Animator_3D()->IsAnimFinish())
	{
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 66;
	}

	if (KEY_INPUT(KEY_SPACE, STATE_TAB))
	{
		m_eState = PLAYER_STATE::ATK_04_3;
		Animator_3D()->PlayAnimation(L"Atk_04_3");
		m_vPos = Transform()->GetLocalPos();
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 15;
		m_bSuperArmor = false;
		SetTrailActive(false);
	}

	Atk_04_2Collision();
}

void CArgoniteLeader::Atk_04_2Collision()
{
	CLayer* pMonLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Monster");

	list<CGameObject*>& listMon = pMonLayer->GetParentList();
}

void CArgoniteLeader::Atk_04_3()
{
	if (true == Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		Animator_3D()->SetBlur(false);
		m_vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * 37;
		SetTrailActive(false);
	}
}

void CArgoniteLeader::ReadyAction()
{
	if (m_iCurAnimIdx == 1285 && m_bOnce == false)
	{
		if (m_bAuraOn)
			SetAura(false);
		else
			SetAura(true);

		m_bOnce = true;
		Vec3 vPos = m_vPos;
		Vec3 vDir = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Transform()->GetLocalPos() - vPos; vDir.Normalize();
		vPos += vDir * 20; vPos.y += 40;
		CEffectMgr::GetInst()->ActiveEffect(L"Particle_Blue_02", vPos, Vec3(200, 200, 200), 0.05f);
	}
	

	if (true == Animator_3D()->IsAnimFinish())
	{
		m_eState = PLAYER_STATE::WAIT;
		Animator_3D()->PlayAnimation(L"Wait");
		Animator_3D()->SetBlur(false);
		m_bOnce = false;
	}
}

void CArgoniteLeader::Hit()
{
	CPlayerScript::Hit();

	if (1073 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		if (m_bStun)
		{
			Audio()->Play(L"Player_Stun", 1);
		}
		else
		{
			int iRand = getRandNum_int(0, 2);

			if (0 == iRand)
			{
				Audio()->Play(L"Player_Damage01", 1);
			}
			else if(1 == iRand)
			{
				Audio()->Play(L"Player_Damage02", 1);
			}
			else
			{
				Audio()->Play(L"Player_Damage03", 1);
			}
		}
	}

	if (m_iHP <= 0)
	{
		m_eState = PLAYER_STATE::DEAD;
		Animator_3D()->PlayAnimation(L"Dead");
	}
}

void CArgoniteLeader::Wait()
{
	Animator_3D()->PlayAnimation(L"Wait");
	CheckAttack();

	if (KEY_INPUT(KEY_W, STATE_HOLD) || KEY_INPUT(KEY_S, STATE_HOLD) || KEY_INPUT(KEY_A, STATE_HOLD) || KEY_INPUT(KEY_D, STATE_HOLD) )
	{
		m_eState = PLAYER_STATE::WALK;
		Animator_3D()->PlayAnimation(L"Walk");
	}

	if (KEY_INPUT(KEY_R, STATE_TAB))
	{
		m_eState = PLAYER_STATE::READY_ACTION;
		Animator_3D()->PlayAnimation(L"ReadyAction");
	}

	if (1558 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 2))
		{
		case 0:
			Audio()->Play(L"Player_Wait01", 1);
			break;
		case 1:
			Audio()->Play(L"Player_Wait02", 1);
			break;
		case 2:
			Audio()->Play(L"Player_Wait03", 1);
			break;
		default:
			break;
		}
	}
}

void CArgoniteLeader::Walk()
{
	CPlayerScript::Walk();

	if (1619 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 2))
		{
		case 0:
			Audio()->Play(L"Player_Walk01", 1);
			break;
		case 1:
			Audio()->Play(L"Player_Walk02", 1);
			break;
		case 2:
			Audio()->Play(L"Player_Walk03", 1);
			break;
		default:
			break;
		}
	}
}

void CArgoniteLeader::Run()
{
	CPlayerScript::Run();

	if (1325 == Animator_3D()->GetCurFrameIdx())
	{
		Audio()->Stop();
		switch (getRandNum_int(0, 2))
		{
		case 0:
			Audio()->Play(L"Player_Run01", 1);
			break;
		case 1:
			Audio()->Play(L"Player_Run02", 1);
			break;
		case 2:
			Audio()->Play(L"Player_Run03", 1);
			break;
		default:
			break;
		}
	}
}

void CArgoniteLeader::CreateAfterImage()
{
	CGameObject* pEffect = new CGameObject;
	pEffect->SetTag(L"afterImage");
	pEffect->AddComponent<CTransform>(Transform()->Clone());
	pEffect->AddComponent<CMeshRender>(MeshRender()->Clone());
	pEffect->AddComponent<CAnimator_3D>(new CAnimator_3D);
	pEffect->AddComponent<CScript>(new CBurnOutScript);

	CTexture** ppTex = (CTexture**)pEffect->MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0);

	CMaterial* pMtrl = CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_AfterImage");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, ppTex);
	pEffect->MeshRender()->SetMaterial(pMtrl);
	pEffect->MeshRender()->SetPostEffect(true);
	pEffect->MeshRender()->SetCastShadow(false);
	
	pEffect->Animator_3D()->SetBoneTex(MeshRender()->GetMesh()->GetBoneTex());
	pEffect->Animator_3D()->SetFinalBoneMatrix(Animator_3D()->GetFinalBoneMatrix());
	pEffect->Animator_3D()->SetStop(true);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(pEffect);
}

void CArgoniteLeader::SetTrailTex()
{
	vector<CGameObject*> vecTrailChild;

	list<CGameObject*> listChild = GetGameObject()->GetChildList();
	list<CGameObject*>::iterator iter = listChild.begin();
	for (; iter != listChild.end(); ++iter)
	{
		if ((*iter)->GetTag() == L"Sword")
		{
			(*iter)->Trail()->SetTrailMtrl(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
			(*iter)->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_01", L"Texture\\Trail\\Trail_03.png"));
		}
		else if ((*iter)->GetTag() == L"MotionTrail")
		{
			(*iter)->Trail()->SetTrailMtrl(CResMgr::GetInst()->GetCloneMaterial(L"Mtrl_Trail"));
			(*iter)->Trail()->SetTrailTexture((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_Motion", L"Texture\\Trail\\Trail_White.jpg"));
		}
	}
}

void CArgoniteLeader::MoveLonger(int _iIdx)
{
	Vec3 vDir = {};
	Vec3 vFront = Transform()->GetDir(DIR_TYPE::RIGHT);
	float fRadian = 0.f;
	if (121 < _iIdx && _iIdx < 139)
	{
		fRadian = -(float)M_PI * 0.2f;
		vDir.x = cosf(fRadian) * vFront.x - sinf(fRadian) * vFront.z;
		vDir.z = sinf(fRadian) * vFront.x + cosf(fRadian) * vFront.z;
	}
	else if (151 < _iIdx && _iIdx < 169)
	{
		fRadian = (float)M_PI * -0.4f;
		vDir.x = cosf(fRadian) * vFront.x - sinf(fRadian) * vFront.z;
		vDir.z = sinf(fRadian) * vFront.x + cosf(fRadian) * vFront.z;
		vDir *= -1.4f;
	}
	else if (181 < _iIdx && _iIdx < 199)
	{
		vDir.x = Transform()->GetDir(DIR_TYPE::FRONT).x * -1;
		vDir.z = Transform()->GetDir(DIR_TYPE::FRONT).z * -1;
	}
	else if (208 < _iIdx && _iIdx < 226)
	{
		fRadian = (float)M_PI * 0.3f;
		vDir.x = (cosf(fRadian) * vFront.x - sinf(fRadian) * vFront.z);
		vDir.z = sinf(fRadian) * vFront.x + cosf(fRadian) * vFront.z;
	}
	else if (236 < _iIdx && _iIdx < 254)
	{
		fRadian = (float)M_PI * 0.2f;
		vDir.x = cosf(fRadian) * vFront.x - sinf(fRadian) * vFront.z;
		vDir.z = sinf(fRadian) * vFront.x + cosf(fRadian) * vFront.z;
		vDir *= -1.f;
	}

	m_vPos += vDir * 500 * DT();
}

void CArgoniteLeader::SetAura(bool _bTrue)
{
	m_bAuraOn = _bTrue;
	list<CGameObject*>& listChild = GetGameObject()->GetChildList();
	if (_bTrue)
	{
		for (auto& child : listChild)
		{
			if (child->GetTag() == L"Aura")
			{
				((CAuraScript*)GetGameObject()->FindScript(SCRIPT_AURA))->SetActive(true);
				return;
			}
		}
	}
	
	else
	{
		for (auto& child : listChild)
		{
			if (child->GetTag() == L"Aura")
			{
				((CAuraScript*)GetGameObject()->FindScript(SCRIPT_AURA))->SetActive(false);
				return;
			}
		}
	}
}

void CArgoniteLeader::SetAuraTex()
{
	CAuraScript* pAura = (CAuraScript*)GetGameObject()->FindScript(SCRIPT_AURA);
	if (pAura == nullptr)
		return;
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Aura", L"Texture//Effect//Electric.jpg");
	pAura->SetAuraTex(pTex);
}

