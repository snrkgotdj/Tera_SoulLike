#pragma once

#include "global.h"

class CLayer;
class CGameObject;


class CScene
{
private:
	map<wstring, CLayer*>	m_mapLayer;
	vector<CLayer*>			m_vecLayer;
	set<UINT>				m_setLayerIdx;

public:
	void Awake();
	void Start();
	int Update();
	int LateUpdate();
	int DelayUpdate();
	int FinalUpdate();

	bool Save(const wstring& _strPath);
	bool Load(const wstring& _strPath);

public:
	int AddLayer(const wstring& _strLayerName);
	int AddLayer(CLayer* _pLayer);
	CLayer* FindLayer(const wstring& _strLayerName);
	CLayer* FindLayer(UINT _iIdx);
	CGameObject* FindObject(Vec2 _vWindowPos);
	void GetAllParentObjects(list<CGameObject*>& _listOut);
	UINT GetLayerCount() { return (UINT)m_mapLayer.size(); }
	void DeleteLayer(const wstring& _strKey);
	void DestoryLayer(UINT _iIdx);
	CGameObject* GetUIObject();

private:
	UINT GetLayerIdx();	// 가장 앞쪽 레이어 번호를 가져온다.
	void FindPlayer();

	const vector<CLayer*>& GetVecLayer() { return m_vecLayer; }
	Vec2 Get2DWorldPosFromWindowPos(Vec2 _vWindowPos);

public:
	CScene();
	~CScene();

	friend class CCamera;
	friend class CCollisionMgr;
	friend class CObjectInfoDlg;
	friend class CCameraDlg;
	friend class CSaveLoadMgr;
	friend class CLayerSettingDlg;
};


