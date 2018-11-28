#pragma once

#include "global.h"



struct tCellLine
{
	Vec2 vPointA;
	Vec2 vPointB;

};


class CNaviCell
{
private:
	UINT m_iPatchIdx;

	CNaviCell*	m_pLinkCell[3]; // 인접 네비셀
	Vec3		m_vVertex[3];		// 꼭지점
	Vec3		m_vCenter;
	tCellLine   m_tCellLine[3];
	Vec3		m_vLineMid[3];
	float		m_fLineDist[3];


public:
	void init(Vec3& _vPOINTA, Vec3& _vPOINTB, Vec3& _vPOINTC);
	void SetPatchIdx(UINT _iIdx) { m_iPatchIdx = _iIdx; }
	void SetLinkCell(CELL_SIDE _eSide, CNaviCell* _pCell) { m_pLinkCell[_eSide] = _pCell; }

	UINT GetPatchIdx() { return m_iPatchIdx; }
	CNaviCell* GetLinkCell(CELL_SIDE _eSide) { return m_pLinkCell[_eSide]; }
	const tCellLine& GetCellLine(CELL_SIDE _eSide) { return m_tCellLine[_eSide]; }
	const Vec3& GetLineMid(CELL_SIDE _eSide) { return m_vLineMid[_eSide]; }
	Vec3& GetCenter() { return m_vCenter; }

public:
	void Save(FILE* _pFile);
	void Load(FILE* _pFile);

public:
	CNaviCell();
	~CNaviCell();
};
