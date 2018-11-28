#pragma once

#include "global.h"

class CGameObject;
class CNaviMesh;

class CNavigation
{
private:
	CGameObject* m_pObj;
	CGameObject* m_pTerrainObj;
	Vec3 m_vGoalPos;
	CNaviMesh* m_pNavMesh;
	vector<tPathNode> m_vecPath;
	int m_iStart;
	int m_iGoal;
	bool m_bMove;

	int m_iPathIdx;

public:
	bool MovetoObject(CGameObject* _pStart, CGameObject* _pObj);
	bool MovetoVec3(Vec3& _vPos);

	void SetGameObject(CGameObject* _pObj) { m_pObj = _pObj; }
	bool isOnLoad(Vec3 _vPos);
	void Update(CGameObject* _pObj, float _fSpeed);

private:
	void ResetPath();

public:
	CNavigation();
	~CNavigation();
};

