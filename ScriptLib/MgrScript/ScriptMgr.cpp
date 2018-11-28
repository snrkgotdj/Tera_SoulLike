#include "ScriptMgr.h"

#include "..\AbominationGuard.h"
#include "..\ArgoniteLeader.h"
#include "..\AsuraScript.h"
#include "..\AuraScript.h"
#include "..\BillBoardScript.h"
#include "..\BloodCreature.h"
#include "..\BloodEffect.h"
#include "..\BloodGolem.h"
#include "..\BurnOutScript.h"
#include "..\ButtonScript.h"
#include "..\CameraScript.h"
#include "..\DefaultEffect.h"
#include "..\ElectricEffect.h"
#include "..\EnergyBall.h"
#include "..\LightBoom.h"
#include "..\LightMoveScript.h"
#include "..\MoveScript.h"
#include "..\ShockEarthScript.h"
#include "..\ShockWave.h"
#include "..\SunScript.h"
#include "..\TerrainScript.h"
#include "..\TestScript.h"
#include "..\ThirdCamScript.h"
#include "..\TileEffect.h"
#include "..\WarriorScript.h"
#include "..\WaterScript.h"
#include "..\WaveEffect.h"
#include "..\MgrScript\EffectMgr.h"
#include "..\MgrScript\GameSystemMgr.h"
#include "..\MgrScript\SkillMgr.h"
#include "..\MgrScript\SoundMgr.h"
#include "..\MgrScript\UIMgrScript.h"

void CScriptMgr::GetScriptInfo(vector<tScriptInfo>& _vec)
{
	_vec.push_back(tScriptInfo{ L"AbominationGuard", (DWORD)typeid(CAbominationGuard).hash_code() });
	_vec.push_back(tScriptInfo{ L"ArgoniteLeader", (DWORD)typeid(CArgoniteLeader).hash_code() });
	_vec.push_back(tScriptInfo{ L"AsuraScript", (DWORD)typeid(CAsuraScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"AuraScript", (DWORD)typeid(CAuraScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"BillBoardScript", (DWORD)typeid(CBillBoardScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"BloodCreature", (DWORD)typeid(CBloodCreature).hash_code() });
	_vec.push_back(tScriptInfo{ L"BloodEffect", (DWORD)typeid(CBloodEffect).hash_code() });
	_vec.push_back(tScriptInfo{ L"BloodGolem", (DWORD)typeid(CBloodGolem).hash_code() });
	_vec.push_back(tScriptInfo{ L"BurnOutScript", (DWORD)typeid(CBurnOutScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"ButtonScript", (DWORD)typeid(CButtonScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"CameraScript", (DWORD)typeid(CCameraScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"DefaultEffect", (DWORD)typeid(CDefaultEffect).hash_code() });
	_vec.push_back(tScriptInfo{ L"ElectricEffect", (DWORD)typeid(CElectricEffect).hash_code() });
	_vec.push_back(tScriptInfo{ L"EnergyBall", (DWORD)typeid(CEnergyBall).hash_code() });
	_vec.push_back(tScriptInfo{ L"LightBoom", (DWORD)typeid(CLightBoom).hash_code() });
	_vec.push_back(tScriptInfo{ L"LightMoveScript", (DWORD)typeid(CLightMoveScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"MoveScript", (DWORD)typeid(CMoveScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"ShockEarthScript", (DWORD)typeid(CShockEarthScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"ShockWave", (DWORD)typeid(CShockWave).hash_code() });
	_vec.push_back(tScriptInfo{ L"SunScript", (DWORD)typeid(CSunScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"TerrainScript", (DWORD)typeid(CTerrainScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"TestScript", (DWORD)typeid(CTestScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"ThirdCamScript", (DWORD)typeid(CThirdCamScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"TileEffect", (DWORD)typeid(CTileEffect).hash_code() });
	_vec.push_back(tScriptInfo{ L"WarriorScript", (DWORD)typeid(CWarriorScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"WaterScript", (DWORD)typeid(CWaterScript).hash_code() });
	_vec.push_back(tScriptInfo{ L"WaveEffect", (DWORD)typeid(CWaveEffect).hash_code() });
}

CScript * CScriptMgr::GetScript(DWORD _dwTypeId)
{
	if (typeid(CAbominationGuard).hash_code() == _dwTypeId)
		return new CAbominationGuard;
	if (typeid(CArgoniteLeader).hash_code() == _dwTypeId)
		return new CArgoniteLeader;
	if (typeid(CAsuraScript).hash_code() == _dwTypeId)
		return new CAsuraScript;
	if (typeid(CAuraScript).hash_code() == _dwTypeId)
		return new CAuraScript;
	if (typeid(CBillBoardScript).hash_code() == _dwTypeId)
		return new CBillBoardScript;
	if (typeid(CBloodCreature).hash_code() == _dwTypeId)
		return new CBloodCreature;
	if (typeid(CBloodEffect).hash_code() == _dwTypeId)
		return new CBloodEffect;
	if (typeid(CBloodGolem).hash_code() == _dwTypeId)
		return new CBloodGolem;
	if (typeid(CBurnOutScript).hash_code() == _dwTypeId)
		return new CBurnOutScript;
	if (typeid(CButtonScript).hash_code() == _dwTypeId)
		return new CButtonScript;
	if (typeid(CCameraScript).hash_code() == _dwTypeId)
		return new CCameraScript;
	if (typeid(CDefaultEffect).hash_code() == _dwTypeId)
		return new CDefaultEffect;
	if (typeid(CElectricEffect).hash_code() == _dwTypeId)
		return new CElectricEffect;
	if (typeid(CEnergyBall).hash_code() == _dwTypeId)
		return new CEnergyBall;
	if (typeid(CLightBoom).hash_code() == _dwTypeId)
		return new CLightBoom;
	if (typeid(CLightMoveScript).hash_code() == _dwTypeId)
		return new CLightMoveScript;
	if (typeid(CMoveScript).hash_code() == _dwTypeId)
		return new CMoveScript;
	if (typeid(CShockEarthScript).hash_code() == _dwTypeId)
		return new CShockEarthScript;
	if (typeid(CShockWave).hash_code() == _dwTypeId)
		return new CShockWave;
	if (typeid(CSunScript).hash_code() == _dwTypeId)
		return new CSunScript;
	if (typeid(CTerrainScript).hash_code() == _dwTypeId)
		return new CTerrainScript;
	if (typeid(CTestScript).hash_code() == _dwTypeId)
		return new CTestScript;
	if (typeid(CThirdCamScript).hash_code() == _dwTypeId)
		return new CThirdCamScript;
	if (typeid(CTileEffect).hash_code() == _dwTypeId)
		return new CTileEffect;
	if (typeid(CWarriorScript).hash_code() == _dwTypeId)
		return new CWarriorScript;
	if (typeid(CWaterScript).hash_code() == _dwTypeId)
		return new CWaterScript;
	if (typeid(CWaveEffect).hash_code() == _dwTypeId)
		return new CWaveEffect;
	return nullptr;
}

CScript * CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"AbominationGuard" == _strScriptName)
		return new CAbominationGuard;
	if (L"ArgoniteLeader" == _strScriptName)
		return new CArgoniteLeader;
	if (L"AsuraScript" == _strScriptName)
		return new CAsuraScript;
	if (L"AuraScript" == _strScriptName)
		return new CAuraScript;
	if (L"BillBoardScript" == _strScriptName)
		return new CBillBoardScript;
	if (L"BloodCreature" == _strScriptName)
		return new CBloodCreature;
	if (L"BloodEffect" == _strScriptName)
		return new CBloodEffect;
	if (L"BloodGolem" == _strScriptName)
		return new CBloodGolem;
	if (L"BurnOutScript" == _strScriptName)
		return new CBurnOutScript;
	if (L"ButtonScript" == _strScriptName)
		return new CButtonScript;
	if (L"CameraScript" == _strScriptName)
		return new CCameraScript;
	if (L"DefaultEffect" == _strScriptName)
		return new CDefaultEffect;
	if (L"ElectricEffect" == _strScriptName)
		return new CElectricEffect;
	if (L"EnergyBall" == _strScriptName)
		return new CEnergyBall;
	if (L"LightBoom" == _strScriptName)
		return new CLightBoom;
	if (L"LightMoveScript" == _strScriptName)
		return new CLightMoveScript;
	if (L"MoveScript" == _strScriptName)
		return new CMoveScript;
	if (L"ShockEarthScript" == _strScriptName)
		return new CShockEarthScript;
	if (L"ShockWave" == _strScriptName)
		return new CShockWave;
	if (L"SunScript" == _strScriptName)
		return new CSunScript;
	if (L"TerrainScript" == _strScriptName)
		return new CTerrainScript;
	if (L"TestScript" == _strScriptName)
		return new CTestScript;
	if (L"ThirdCamScript" == _strScriptName)
		return new CThirdCamScript;
	if (L"TileEffect" == _strScriptName)
		return new CTileEffect;
	if (L"WarriorScript" == _strScriptName)
		return new CWarriorScript;
	if (L"WaterScript" == _strScriptName)
		return new CWaterScript;
	if (L"WaveEffect" == _strScriptName)
		return new CWaveEffect;
	if (L"EffectMgr" == _strScriptName)
		return new CEffectMgr;
	if (L"GameSystemMgr" == _strScriptName)
		return new CGameSystemMgr;
	if (L"SkillMgr" == _strScriptName)
		return new CSkillMgr;
	if (L"SoundMgr" == _strScriptName)
		return new CSoundMgr;
	if (L"UIMgrScript" == _strScriptName)
		return new CUIMgrScript;
	return nullptr;
}

const wchar_t * CScriptMgr::GetScriptName(CScript * _pScript)
{
	if (NULL != dynamic_cast<CAbominationGuard*>(_pScript))
		return L"AbominationGuard";
	if (NULL != dynamic_cast<CArgoniteLeader*>(_pScript))
		return L"ArgoniteLeader";
	if (NULL != dynamic_cast<CAsuraScript*>(_pScript))
		return L"AsuraScript";
	if (NULL != dynamic_cast<CAuraScript*>(_pScript))
		return L"AuraScript";
	if (NULL != dynamic_cast<CBillBoardScript*>(_pScript))
		return L"BillBoardScript";
	if (NULL != dynamic_cast<CBloodCreature*>(_pScript))
		return L"BloodCreature";
	if (NULL != dynamic_cast<CBloodEffect*>(_pScript))
		return L"BloodEffect";
	if (NULL != dynamic_cast<CBloodGolem*>(_pScript))
		return L"BloodGolem";
	if (NULL != dynamic_cast<CBurnOutScript*>(_pScript))
		return L"BurnOutScript";
	if (NULL != dynamic_cast<CButtonScript*>(_pScript))
		return L"ButtonScript";
	if (NULL != dynamic_cast<CCameraScript*>(_pScript))
		return L"CameraScript";
	if (NULL != dynamic_cast<CDefaultEffect*>(_pScript))
		return L"DefaultEffect";
	if (NULL != dynamic_cast<CElectricEffect*>(_pScript))
		return L"ElectricEffect";
	if (NULL != dynamic_cast<CEnergyBall*>(_pScript))
		return L"EnergyBall";
	if (NULL != dynamic_cast<CLightBoom*>(_pScript))
		return L"LightBoom";
	if (NULL != dynamic_cast<CLightMoveScript*>(_pScript))
		return L"LightMoveScript";
	if (NULL != dynamic_cast<CMoveScript*>(_pScript))
		return L"MoveScript";
	if (NULL != dynamic_cast<CShockEarthScript*>(_pScript))
		return L"ShockEarthScript";
	if (NULL != dynamic_cast<CShockWave*>(_pScript))
		return L"ShockWave";
	if (NULL != dynamic_cast<CSunScript*>(_pScript))
		return L"SunScript";
	if (NULL != dynamic_cast<CTerrainScript*>(_pScript))
		return L"TerrainScript";
	if (NULL != dynamic_cast<CTestScript*>(_pScript))
		return L"TestScript";
	if (NULL != dynamic_cast<CThirdCamScript*>(_pScript))
		return L"ThirdCamScript";
	if (NULL != dynamic_cast<CTileEffect*>(_pScript))
		return L"TileEffect";
	if (NULL != dynamic_cast<CWarriorScript*>(_pScript))
		return L"WarriorScript";
	if (NULL != dynamic_cast<CWaterScript*>(_pScript))
		return L"WaterScript";
	if (NULL != dynamic_cast<CWaveEffect*>(_pScript))
		return L"WaveEffect";
	if (NULL != dynamic_cast<CEffectMgr*>(_pScript))
		return L"EffectMgr";
	if (NULL != dynamic_cast<CGameSystemMgr*>(_pScript))
		return L"GameSystemMgr";
	if (NULL != dynamic_cast<CSkillMgr*>(_pScript))
		return L"SkillMgr";
	if (NULL != dynamic_cast<CSoundMgr*>(_pScript))
		return L"SoundMgr";
	if (NULL != dynamic_cast<CUIMgrScript*>(_pScript))
		return L"UIMgrScript";
	return nullptr;
}

