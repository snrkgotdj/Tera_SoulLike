#include "AuraScript.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "Animator_3D.h"

#include "TimeMgr.h"

#include "ResMgr.h"
#include "Material.h"

CAuraScript::CAuraScript()
	: m_pAura(nullptr)
	, m_bOn(false)
	, m_bActive(false)
	, m_fTime(0.f)
{
	m_ID = SCRIPT_AURA;
}

CAuraScript::~CAuraScript()
{
}

void CAuraScript::init()
{
	if (nullptr != m_pAura)
		return;

	CGameObject* pObj = GetGameObject();
	list<CGameObject*> PlayerChild = pObj->GetChildList();
	for (auto& iter : PlayerChild)
	{
		if (iter->GetTag() == L"Aura")
		{
			iter->Animator_3D()->SetFinalBoneMatrixPtr(pObj->Animator_3D()->GetFinalBoneMatPtr());
			iter->SetAnimationActive(false);
			m_pAura = iter;
			return;
		}
	}

	m_pAura = GetGameObject()->Clone();
	m_pAura->SetTag(L"Aura");
	m_pAura->Transform()->SetLocalScale(Vec3(1.1f, 1.f, 1.f));	m_pAura->MeshRender()->SetPostEffect(true);
	m_pAura->MeshRender()->SetCastShadow(false);
	m_pAura->DeleteAllScript();
	m_pAura->SetAnimationActive(false);
	m_pAura->Animator_3D()->SetFinalBoneMatrixPtr(pObj->Animator_3D()->GetFinalBoneMatPtr());

	list<CGameObject*> listChild = m_pAura->GetChildList();
	list<CGameObject*>::iterator iter = listChild.begin();
	for (; iter != listChild.end(); ++iter)
	{
		(*iter)->Destroy();
	}

	CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Electric_Effect");
	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Aura", L"Texture//Effect//Electric.jpg");
	pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTex);

	int iMtrlSize = m_pAura->MeshRender()->GetMaterialSize();
	for (int i = 0; i < iMtrlSize; ++i)
	{
		m_pAura->MeshRender()->SetMaterial(pMtrl,i);
	}

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Effect")->AddGameObject(m_pAura);
	GetGameObject()->AddChildObj(m_pAura);
}

void CAuraScript::Awake()
{
	init();

	list<CGameObject*>& listChild = GetGameObject()->GetChildList();
	for (auto& pIter : listChild)
	{
		if (L"Aura" == pIter->GetTag())
		{
			m_pAura = pIter;
			break;
		}
	}
	m_pAura->Animator_3D()->SetPlay(true);
	m_pAura->MeshRender()->SetCastShadow(false);

	return;
}

void CAuraScript::Start()
{
	
	
	return;
}

int CAuraScript::Update()
{
	if (nullptr == m_pAura)
		return 0;

	m_pAura->Transform()->SetLocalPos(Vec3(0, 0, 0));

	if (m_bActive != m_bOn)
	{
		float fData = 0.f;
		m_fTime += DT();
		
		if (m_bActive)
		{
			fData = m_fTime;
		}
		else
		{
			fData = 1 - m_fTime;
		}
		for (int i = 0; i < m_pAura->MeshRender()->GetMaterialSize(); ++i)
		{
			m_pAura->MeshRender()->GetSharedMaterial(i)->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);
		}
	}

	if (m_fTime >= 1.f)
	{
		m_bOn = m_bActive;
		m_fTime = 0.f;
	}


	return 0;
}

void CAuraScript::SetAuraTex(CTexture * _pTex)
{
	for (int i = 0; i < m_pAura->MeshRender()->GetMaterialSize(); ++i)
	{
		m_pAura->MeshRender()->GetMaterial(i)->SetData(SHADER_PARAM::SP_TEX_1, &_pTex);
	}
	
}

void CAuraScript::KeyInput()
{
	if (KEY_INPUT(KEY_K, STATE_TAB))
	{
		m_bOn = true;
	}

	if (KEY_INPUT(KEY_L, STATE_TAB))
	{
		m_bOn = false;
	}

	m_pAura->SetActive(m_bOn);

	return;
}