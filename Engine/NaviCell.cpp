#include "NaviCell.h"

CNaviCell::CNaviCell()
	: m_pLinkCell{}
	, m_vVertex{}
	, m_vCenter(0,0,0)
	, m_tCellLine {}
	, m_vLineMid {}
	, m_fLineDist {}
{	 
}

CNaviCell::~CNaviCell()
{
}

void CNaviCell::init(Vec3& _vPointA, Vec3& _vPointB, Vec3& _vPointC)
{
	m_vVertex[VERT_A] = _vPointA;
	m_vVertex[VERT_B] = _vPointB;
	m_vVertex[VERT_C] = _vPointC;

	Vec2 Point1(m_vVertex[VERT_A].x, m_vVertex[VERT_A].z);
	Vec2 Point2(m_vVertex[VERT_B].x, m_vVertex[VERT_B].z);
	Vec2 Point3(m_vVertex[VERT_C].x, m_vVertex[VERT_C].z);

	// innitialize our sides
	m_tCellLine[SIDE_AB].vPointA = Point1;
	m_tCellLine[SIDE_AB].vPointB = Point2;

	m_tCellLine[SIDE_BC].vPointA = Point2;
	m_tCellLine[SIDE_BC].vPointB = Point3;

	m_tCellLine[SIDE_CA].vPointA = Point3;
	m_tCellLine[SIDE_CA].vPointB = Point1;

	// compute midpoint as centroid of polygon
	m_vCenter.x = ((m_vVertex[VERT_A].x + m_vVertex[VERT_B].x + m_vVertex[VERT_C].x) / 3);
	m_vCenter.y = ((m_vVertex[VERT_A].y + m_vVertex[VERT_B].y + m_vVertex[VERT_C].y) / 3);
	m_vCenter.z = ((m_vVertex[VERT_A].z + m_vVertex[VERT_B].z + m_vVertex[VERT_C].z) / 3);

	// compute the midpoint of each cell wall
	m_vLineMid[0].x = (m_vVertex[VERT_A].x + m_vVertex[VERT_B].x) / 2.0f;
	m_vLineMid[0].y = (m_vVertex[VERT_A].y + m_vVertex[VERT_B].y) / 2.0f;
	m_vLineMid[0].z = (m_vVertex[VERT_A].z + m_vVertex[VERT_B].z) / 2.0f;

	m_vLineMid[1].x = (m_vVertex[VERT_C].x + m_vVertex[VERT_B].x) / 2.0f;
	m_vLineMid[1].y = (m_vVertex[VERT_C].y + m_vVertex[VERT_B].y) / 2.0f;
	m_vLineMid[1].z = (m_vVertex[VERT_C].z + m_vVertex[VERT_B].z) / 2.0f;

	m_vLineMid[2].x = (m_vVertex[VERT_C].x + m_vVertex[VERT_A].x) / 2.0f;
	m_vLineMid[2].y = (m_vVertex[VERT_C].y + m_vVertex[VERT_A].y) / 2.0f;
	m_vLineMid[2].z = (m_vVertex[VERT_C].z + m_vVertex[VERT_A].z) / 2.0f;

	// compute the distances between the wall midpoints
	Vec3 WallVector;
	WallVector = m_vLineMid[0] - m_vLineMid[1];
	m_fLineDist[0] = WallVector.Distance();

	WallVector = m_vLineMid[1] - m_vLineMid[2];
	m_fLineDist[1] = WallVector.Distance();

	WallVector = m_vLineMid[2] - m_vLineMid[0];
	m_fLineDist[2] = WallVector.Distance();
}

void CNaviCell::Save(FILE* _pFile)
{
	fwrite(&m_iPatchIdx, sizeof(UINT), 1, _pFile);	
	fwrite(&m_vVertex, sizeof(Vec3) , 3, _pFile);
	fwrite(&m_vCenter, sizeof(Vec3) , 1, _pFile);
	fwrite(&m_tCellLine, sizeof(tCellLine), 3, _pFile);
	fwrite(&m_vLineMid, sizeof(Vec3), 3, _pFile);
	fwrite(&m_fLineDist, sizeof(float), 3, _pFile);
}

void CNaviCell::Load(FILE* _pFile)
{
	fread(&m_iPatchIdx, sizeof(UINT), 1, _pFile);
	fread(&m_vVertex, sizeof(Vec3), 3, _pFile);
	fread(&m_vCenter, sizeof(Vec3), 1, _pFile);
	fread(&m_tCellLine, sizeof(tCellLine), 3, _pFile);
	fread(&m_vLineMid, sizeof(Vec3), 3, _pFile);
	fread(&m_fLineDist, sizeof(float), 3, _pFile);
}
