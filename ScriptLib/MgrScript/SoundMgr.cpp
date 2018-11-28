#include "SoundMgr.h"

#include "Sound.h"

CSoundMgr::CSoundMgr()
{
	m_ID = SCRIPT_SOUNDMGR;
}

CSoundMgr::~CSoundMgr()
{

}

void CSoundMgr::Init()
{
	// BGM
	CResMgr::GetInst()->LoadSound(L"Stage1_Bgm", L"Sound\\BGM\\Stage1_Bgm.mp3");
	CResMgr::GetInst()->LoadSound(L"Boss_Bgm", L"Sound\\BGM\\Boss_Bgm.mp3");

	// Effect
	CResMgr::GetInst()->LoadSound(L"Sword_Swing", L"Sound\\Effect\\Sword_Swing.flac", true);
	CResMgr::GetInst()->LoadSound(L"Explosion", L"Sound\\Effect\\Explosion.wav", true);
	CResMgr::GetInst()->LoadSound(L"Hit_01", L"Sound\\Player\\Hit_01.mp3", true);
	CResMgr::GetInst()->LoadSound(L"Hit_02", L"Sound\\Player\\Hit_02.mp3", true);
	CResMgr::GetInst()->LoadSound(L"Hit_03", L"Sound\\Player\\Hit_03.mp3", true);
	CResMgr::GetInst()->LoadSound(L"Hit_04", L"Sound\\Player\\Hit_04.mp3", true);

	// Player
	CResMgr::GetInst()->LoadSound(L"Player_Atk01_1", L"Sound\\Player\\Player_Atk01_1.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Atk01_2", L"Sound\\Player\\Player_Atk01_2.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Damage01", L"Sound\\Player\\Player_Damage01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Damage02", L"Sound\\Player\\Player_Damage02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Damage03", L"Sound\\Player\\Player_Damage03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Run01", L"Sound\\Player\\Player_Run01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Run02", L"Sound\\Player\\Player_Run02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Run03", L"Sound\\Player\\Player_Run03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Wait01", L"Sound\\Player\\Player_Wait01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Wait02", L"Sound\\Player\\Player_Wait02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Wait03", L"Sound\\Player\\Player_Wait03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Walk01", L"Sound\\Player\\Player_Walk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Walk02", L"Sound\\Player\\Player_Walk02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Player_Walk03", L"Sound\\Player\\Player_Walk03.ogg", true);
	

	// Boss
	CResMgr::GetInst()->LoadSound(L"Boss_Atk01", L"Sound\\Boss\\Boss_Atk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Atk02", L"Sound\\Boss\\Boss_Atk02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_HeavyAtk", L"Sound\\Boss\\Boss_HeavyAtk.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_LongAtk", L"Sound\\Boss\\Boss_LongAtk.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_UltraAtk", L"Sound\\Boss\\Boss_UltraAtk.ogg", true);

	CResMgr::GetInst()->LoadSound(L"Boss_Damage", L"Sound\\Boss\\Boss_Damage.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Death", L"Sound\\Boss\\Boss_Death.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Modealram", L"Sound\\Boss\\Boss_Modealram.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Spawn", L"Sound\\Boss\\Boss_Spawn.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Wait01", L"Sound\\Boss\\Boss_Wait01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Wait02", L"Sound\\Boss\\Boss_Wait02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Walk01", L"Sound\\Boss\\Boss_Walk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Boss_Walk02", L"Sound\\Boss\\Boss_Walk02.ogg", true);

	// Monster

	// Creature
	CResMgr::GetInst()->LoadSound(L"Creature_Walk01", L"Sound\\Monster\\Creature\\Creature_Walk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Walk02", L"Sound\\Monster\\Creature\\Creature_Walk02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Walk03", L"Sound\\Monster\\Creature\\Creature_Walk03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Walk04", L"Sound\\Monster\\Creature\\Creature_Walk04.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Atk01_0", L"Sound\\Monster\\Creature\\Creature_Atk01_0.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Atk01_1", L"Sound\\Monster\\Creature\\Creature_Atk01_1.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Damages01", L"Sound\\Monster\\Creature\\Creature_Damages01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Damages02", L"Sound\\Monster\\Creature\\Creature_Damages02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Damages03", L"Sound\\Monster\\Creature\\Creature_Damages03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Creature_Death", L"Sound\\Monster\\Creature\\Creature_Death.ogg", true);

	// Golem
	CResMgr::GetInst()->LoadSound(L"Golem_atk01", L"Sound\\Monster\\Golem\\Golem_atk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_atk02", L"Sound\\Monster\\Golem\\Golem_atk02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_atk03", L"Sound\\Monster\\Golem\\Golem_atk03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_atk04", L"Sound\\Monster\\Golem\\Golem_atk04.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_Death", L"Sound\\Monster\\Golem\\Golem_Death.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_run_1", L"Sound\\Monster\\Golem\\Golem_run_1.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Golem_run_2", L"Sound\\Monster\\Golem\\Golem_run_2.ogg", true);

	// Guard
	CResMgr::GetInst()->LoadSound(L"Guard_Attack01_1", L"Sound\\Monster\\Guard\\Guard_Attack01_1.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Attack01_2", L"Sound\\Monster\\Guard\\Guard_Attack01_2.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Damage01", L"Sound\\Monster\\Guard\\Guard_Damage01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Damage02", L"Sound\\Monster\\Guard\\Guard_Damage02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Damage03", L"Sound\\Monster\\Guard\\Guard_Damage03.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Death01", L"Sound\\Monster\\Guard\\Guard_Death01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Death02", L"Sound\\Monster\\Guard\\Guard_Death02.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Walk01", L"Sound\\Monster\\Guard\\Guard_Walk01.ogg", true);
	CResMgr::GetInst()->LoadSound(L"Guard_Walk02", L"Sound\\Monster\\Guard\\Guard_Walk02.ogg", true);
}