#pragma once

#include "global.h"

class CGameObject;

class CLayer
{
private:
	wstring				m_strLayerName;
	list<CGameObject*>	m_listObj;
	list<CGameObject*>	m_listParentObj;

	int					m_iIdx;
	bool				m_bZOrder;
	bool				m_bDestroy;

public:
	void Awake();
	void Start();
	int Update();
	int LateUpdate();
	int DelayUpdate();
	int FinalUpdate();

private:
	void SetLayerIdx(UINT _iLayerIdx) { m_iIdx = _iLayerIdx; }
	void SetLayerName(const wstring& _strLayerName) { m_strLayerName = _strLayerName; }
	list<CGameObject*>& GetObjList() { return m_listObj; }
	list<CGameObject*>& GetParentList() { return m_listParentObj; }
	void SetZOrder(bool _bZOrder) {m_bZOrder = _bZOrder;}
	void RemoveParentObj(CGameObject* _pObj);
	void RemoveObj(CGameObject* _pObj);					// 다른 레이어의 오브젝트
	void DestroyObject(CGameObject* _pObj);

public:
	UINT GetLayerIdx() { return m_iIdx; }
	const wstring& GetLayerName() { return m_strLayerName; }
	void AddGameObject(CGameObject* _pObj, bool _bAll = true);
	CGameObject* FindObjectByTag(const wstring& _strTag);
	CGameObject* FindObject(const Vec2 _vWorldPos);
	CGameObject* FindObjectByRay(const tRay& _tRay);
	void AddParentObject(CGameObject* _pParentObj);
	void Destroy() { m_bDestroy = true; }
	bool IsDestroy() { return m_bDestroy; }
	void DestroyObjAll();
	UINT GetLayerObjSize() { return m_listObj.size(); }

public:
	CLayer();
	~CLayer();

	friend class CScene;
	friend class CRenderMgr;
	friend class CGameObject;
	friend class CCamera;
	friend class CCollisionMgr;
	friend class CSaveLoadMgr;
	friend class CArgoniteLeader;
};

