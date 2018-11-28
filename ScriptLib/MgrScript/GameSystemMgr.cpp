#include "GameSystemMgr.h"

#include "PathMgr.h"
#include "CollisionMgr.h"
#include "Collider_3D.h"

#include "Audio.h"

#include "SkillMgr.h"
#include "EffectMgr.h"
#include "SoundMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

#include "..\External\SaveLoadMgr.h"

#include "../AsuraScript.h"
#include "../AuraScript.h"
#include "../BloodGolem.h"
#include "../BloodCreature.h"
#include "../AbominationGuard.h"

CGameSystemMgr::CGameSystemMgr()
	: m_eStage(STAGE::START)
	, m_iCurMonsterCount(0)
	, m_pAsura(NULL)
{
	m_ID = SCRIPT_GAMESYSTEM;
}

CGameSystemMgr::~CGameSystemMgr()
{
}

void CGameSystemMgr::Awake()
{
	CEffectMgr::GetInst()->LoadEffect(L"Effect");
	GetGameObject()->AddComponent<CAudio>(new CAudio(SOUND_TYPE::TYPE_2D));

	CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetHDRFilter(CRenderMgr::GetInst()->FindFilter(L"Filter_HDR"));
}

int CGameSystemMgr::Update()
{
	if (0 < m_iCurMonsterCount)
		return 0;
	else
		m_eStage = (STAGE)((UINT)m_eStage + 1);

	if (m_eStage < STAGE::STAGE_BOSS)
	{
		Audio()->Play(L"Stage1_Bgm");
		Audio()->SetVolume(0.5f);

		//StartStage(m_eStage);
	}
	else if (STAGE::STAGE_BOSS == m_eStage)
	{
		Audio()->Play(L"Boss_Bgm");
		StartBossStage();
	}
	else if (STAGE::END == m_eStage)
	{

	}

	return 0;
}

void CGameSystemMgr::Init()
{
	//CSceneMgr::GetInst()->LoadFbx();
	CSceneMgr::GetInst()->LoadMData();

	CSkillMgr::GetInst()->Init();
	CEffectMgr::GetInst()->Init();
	CSoundMgr::GetInst()->Init();

	m_arrMonsterTag[(UINT)MONSTER_TYPE::GOLEN] = L"BloodGolem.mdat";
	m_arrMonsterTag[(UINT)MONSTER_TYPE::GUARD] = L"AbominationGuard.mdat";
	m_arrMonsterTag[(UINT)MONSTER_TYPE::CREATURE] = L"BloodCreature.mdat";

	wstring strScenePath = CPathMgr::GetResPath();
	strScenePath += L"Scene\\Main.scene";
	CSaveLoadMgr::GetInst()->LoadScene(strScenePath);
	CSceneMgr::GetInst()->PlayScene();

	//CSceneMgr::GetInst()->GetCurScene()->AddLayer(L"Rock");
	//CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->LayerRenderCheck(L"Rock");

	// 메인카메라에 디폴트로 Depth Field 적용
	//CCamFilter* pFilter = CRenderMgr::GetInst()->FindFilter(L"Filter_DepthOfField");
	//CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Camera()->SetFilter(pFilter);

}

bool CGameSystemMgr::IsBossSpawnTime()
{
	if (NULL == m_pAsura)
		return false;

	BOSS_STATE eState = m_pAsura->GetState();

	if (BOSS_STATE::SPAWN == eState)
		return true;
	if (BOSS_STATE::SPAWN < eState)
		m_pAsura = NULL;

	return false;
}

void CGameSystemMgr::StartStage(const STAGE & _eStage)
{
	Vec3 vPos = Vec3(9800.f, 1.f, 7700.f);
	Vec3 vSpawnPos;

	UINT iMonCount = 0;

	switch (_eStage)
	{
	case STAGE::START:
		break;
	case STAGE::STAGE_1:
		vPos = Vec3(17000.f, 1.f, 1500.f);

		for (UINT i = 0; i < 5; ++i)
		{
			vSpawnPos = vPos;
			vSpawnPos.x = getRandNum_float(7800.f, 11800.f);
			vSpawnPos.z = getRandNum_float(5700.f, 9700.f);
			CreateMonster(MONSTER_TYPE::GUARD, vSpawnPos);
		}

		for (UINT i = 0; i < 3; ++i)
		{
			vSpawnPos = vPos;
			vSpawnPos.x = getRandNum_float(7800.f, 11800.f);
			vSpawnPos.z = getRandNum_float(5700.f, 9700.f);
			CreateMonster(MONSTER_TYPE::GOLEN, vSpawnPos);
		}

		for (UINT i = 0; i < 5; ++i)
		{
			vSpawnPos = vPos;
			vSpawnPos.x = getRandNum_float(7800.f, 11800.f);
			vSpawnPos.z = getRandNum_float(5700.f, 9700.f);
			CreateMonster(MONSTER_TYPE::CREATURE, vSpawnPos);
		}
		break;
	case STAGE::END:
		break;
	}
}

void CGameSystemMgr::StartBossStage()
{
	CPrefab* pPrefab = CResMgr::GetInst()->FindPrefab(L"Player.mdat");
	CGameObject* pObj = Instantiate(pPrefab, Vec3(1.f, 0.f, 1.f), L"Monster");
	pObj->SetTag(L"Boss");
	m_iCurMonsterCount++;

	CCollider_3D* pCol = new CCollider_3D;
	pCol->SetColType(COLLIDER_TYPE::OBB);

	pCol->SetOffsetPos(Vec3(0.f, 100.f, 0.f));
	pCol->SetOffsetScale(Vec3(100.f, 200.f, 200.f));

	CScript* pScript = new CAuraScript;
	pObj->AddScript(SCRIPT_AURA, pScript);

	pScript->Awake();
	pScript->Start();

	pScript = new CAsuraScript;
	pObj->AddComponent<CCollider>(pCol);
	pObj->AddScript(SCRIPT_ASURA, pScript);

	pScript->Awake();
	pScript->Start();

	m_pAsura = (CAsuraScript*)pObj->FindScript(SCRIPT_ASURA);
}

void CGameSystemMgr::CreateMonster(const MONSTER_TYPE& _eType, const Vec3 & _vPos)
{
	CPrefab* pPrefab = CResMgr::GetInst()->FindPrefab(m_arrMonsterTag[(UINT)_eType]);
	CGameObject* pObj = Instantiate(pPrefab, _vPos, L"Monster");
	m_iCurMonsterCount++;

	CMonsterScript* pScript = NULL;
	SCRIPT_ID eScript = SCRIPT_ID::SCRIPT_END;
	CCollider_3D* pCol = new CCollider_3D;
	pCol->SetColType(COLLIDER_TYPE::OBB);

	switch (_eType)
	{
	case MONSTER_TYPE::GOLEN:
		pCol->SetOffsetPos(Vec3(0.f, 100.f, 0.f));
		pCol->SetOffsetScale(Vec3(100.f, 150.f, 100.f));

		pScript = new CBloodGolem;
		eScript = SCRIPT_BLOODGOLEM;
		break;
	case MONSTER_TYPE::CREATURE:
		pCol->SetOffsetPos(Vec3(0.f, 25.f, 0.f));
		pCol->SetOffsetScale(Vec3(35.f, 50.f, 35.f));
		pObj->Transform()->SetLocalScale(Vec3(2.f, 2.f, 2.f));

		pScript = new CBloodCreature;
		eScript = SCRIPT_CREATURE;
		break;
	case MONSTER_TYPE::GUARD:
		pCol->SetOffsetPos(Vec3(0.f, 25.f, 0.f));
		pCol->SetOffsetScale(Vec3(35.f, 50.f, 35.f));
		pObj->Transform()->SetLocalScale(Vec3(1.5f, 1.5f, 1.5f));

		pScript = new CAbominationGuard;
		eScript = SCRIPT_GUARD;
		break;
	}

	pObj->AddComponent<CCollider>(pCol);

	pObj->AddScript(eScript, pScript);
	pScript->Awake();
	pScript->Start();
}

void CGameSystemMgr::Save(FILE * _pFile)
{
	UINT iCount = 0;

	for (UINT i = 0; i < (UINT)MONSTER_TYPE::END; ++i)
	{
		if (false == m_arrMonsterTag[i].empty())
		{
			++iCount;
		}
	}
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(m_arrMonsterTag[i], _pFile);
	}
}

void CGameSystemMgr::Load(FILE * _pFile)
{
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		m_arrMonsterTag[i] = LoadWString(_pFile);
	}

	g_pThis = this;

	m_arrMonsterTag[(UINT)MONSTER_TYPE::GOLEN] = L"BloodGolem.mdat";
	m_arrMonsterTag[(UINT)MONSTER_TYPE::GUARD] = L"AbominationGuard.mdat";
	m_arrMonsterTag[(UINT)MONSTER_TYPE::CREATURE] = L"BloodCreature.mdat";
}