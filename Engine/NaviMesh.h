#pragma once
#include "global.h"

class CNaviCell;

class CNaviMesh 
{
private:
	map<UINT, CNaviCell*> m_mapNaviCell;
	UINT m_iFaceX;
	UINT m_iFaceZ;

public:
	bool AddCell(Vec3& vPointA, Vec3& vPointB, Vec3& vPointC, UINT _iPatchIdx);
	bool AddCell(CNaviCell* _pCell);

	void SetFaceCount(UINT _iFaceX, UINT _iFaceZ);
	CNaviCell* FindNaviCell(UINT _iIdx);
	

private:
	void LinkNavi(CNaviCell* _pNewCell, UINT _iIdx, CELL_SIDE _Link);
	bool FindPath(UINT _iStartPatch, UINT _iGoalPatch, vector<tPathNode>* _vecPath);

public:
	void Save(FILE* _pFile);
	void Load(FILE* _pFile);

public:
	CNaviMesh();
	virtual ~CNaviMesh();

	friend class CNaviMgr;
	friend class CNavigation;
};

