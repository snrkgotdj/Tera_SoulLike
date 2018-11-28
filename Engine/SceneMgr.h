#pragma once

#include "global.h"

class CScene;
class CGameObject;

class CSceneMgr
{
	
private:
	CScene*			m_pCurScene;
	CScene*			m_pPrevScene;

	bool			m_bUpdateEvent;
	bool			m_bPlayMod;
	CGameObject*	m_arrCamObj[(UINT)CAMERA_TYPE::END];
	CGameObject*	m_pTerrainObj;
	CGameObject*	m_pPlayer;

	int (CSceneMgr::*progress_func)(void);

public:
	void SetPlayMod(bool _bTrue);
	bool IsPlayMode() { return m_bPlayMod; }
	void PlayScene();
	void SetCamera(const CAMERA_TYPE& _eType, CGameObject* _pCamObj);
	CGameObject* GetCamera(const CAMERA_TYPE& _eType) { return m_arrCamObj[(UINT)_eType]; }
	void SetTerrainObject(CGameObject* _pObj) { m_pTerrainObj = _pObj; }
	CGameObject* GetTerrainObj() { return m_pTerrainObj; }
	void SetPlayer(CGameObject* _pPlayer) { m_pPlayer = _pPlayer; };
	CGameObject* GetPlayerObj() { return m_pPlayer; }

public:
	void Init();
	int Progress();
	void Render();

private:
	int ProgressPlay();
	int ProgressTool();
	void ChangeFilter();

public:
	CScene* GetCurScene() { return m_pCurScene; }	
	void ChangeScene(CScene* _pNextScene);
	void LoadFbx(const wstring& _strFileName = L"MeshData");
	void LoadMData(const wstring& _strFileName = L"MeshData");
#ifdef _TOOL

	void SetUpdateCheck(bool _bCheck) { m_bUpdateEvent = _bCheck; }
	bool GetUpdateCheck() { return m_bUpdateEvent; }
#endif


	void CreateTestScene();
	void CreateGameObject();

private:
	static CSceneMgr* pInst;

public:
	static CSceneMgr* GetInst()
	{
		if (pInst == nullptr)
			pInst = new CSceneMgr;

		return pInst;
	}
	void DistroyInst() { SAFE_DELETE(pInst); }

	CSceneMgr();
	~CSceneMgr();

};

