#include "Navigation.h"
#include "NaviMesh.h"
#include "NaviCell.h"

#include "SceneMgr.h"
#include "TimeMgr.h"
#include "GameObject.h"
#include "Transform.h"
#include "Terrain.h"

CNavigation::CNavigation()
	: m_iStart(-1)
	, m_iGoal(-1)
	, m_bMove(false)
	, m_pNavMesh(nullptr)
	, m_pTerrainObj(nullptr)
	, m_pObj(nullptr)
{

}

CNavigation::~CNavigation()
{
}

bool CNavigation::MovetoObject(CGameObject* _pStart, CGameObject * _pObj)
{
	m_pObj = _pStart;
	Vec3 vMyPos = m_pObj->Transform()->GetLocalPos();
	m_vGoalPos = _pObj->Transform()->GetLocalPos();


	m_pTerrainObj = CSceneMgr::GetInst()->GetTerrainObj();

	if (nullptr == m_pTerrainObj)
		return false;

	int iStart = m_pTerrainObj->Terrain()->GetPatchIdxByPos(vMyPos);
	int iGoal = m_pTerrainObj->Terrain()->GetPatchIdxByPos(m_vGoalPos);

	if (m_iStart == iStart && m_iGoal == iGoal) // 이전에 계산한것과 동일
		return false;

	if (iStart < 0 || iGoal < 0 || iStart >4095 || iGoal >4095)
	{
		MessageBox(nullptr, L"잘못된 인덱스 접근 예외처리 추가하시요", L"길찾기 오류", MB_OK);
		return false;
	}
	m_pNavMesh = m_pTerrainObj->Terrain()->GetNaviMesh();

	bool bResult = m_pNavMesh->FindPath(iStart, iGoal, &m_vecPath);

	if (bResult == false)
	{
		MessageBox(nullptr, L"그장소는 길이 아닙니다.", L"길찾기 실패", MB_OK);
		return false;
	}

	m_bMove = true;
	m_iPathIdx = 0;

	return true;
}

bool CNavigation::MovetoVec3(Vec3 & _vPos)
{
	Vec3 vMyPos = m_pObj->Transform()->GetLocalPos();
	m_vGoalPos = _vPos;

	m_pTerrainObj = CSceneMgr::GetInst()->GetTerrainObj();

	if (nullptr == m_pTerrainObj)
		return false;

	int iStart = m_pTerrainObj->Terrain()->GetPatchIdxByPos(vMyPos);
	int iGoal = m_pTerrainObj->Terrain()->GetPatchIdxByPos(m_vGoalPos);

	if (m_iStart == iStart && m_iGoal == iGoal) // 이전에 계산한것과 동일
		return true;

	if (iStart < 0 || iGoal < 0 || iStart >4095 || iGoal >4095)
	{
		MessageBox(nullptr, L"잘못된 인덱스 접근 예외처리 추가하시요", L"길찾기 오류", MB_OK);
		return false;
	}
	m_pNavMesh = m_pTerrainObj->Terrain()->GetNaviMesh();

	bool bResult = m_pNavMesh->FindPath(iStart, iGoal, &m_vecPath);

	if (bResult == false)
	{
		MessageBox(nullptr, L"그장소는 길이 아닙니다.", L"길찾기 실패", MB_OK);
		return false;
	}

	m_bMove = true;
	m_iPathIdx = 0;

	return true;
}

bool CNavigation::isOnLoad(Vec3 _vPos)
{
	m_pTerrainObj = CSceneMgr::GetInst()->GetTerrainObj();
	m_pNavMesh = m_pTerrainObj->Terrain()->GetNaviMesh();

	UINT iIdx = m_pTerrainObj->Terrain()->GetPatchIdxByPos(_vPos);
	CNaviCell* pNaviCell = m_pNavMesh->FindNaviCell(iIdx);

	if (nullptr == pNaviCell)
		return false;

	return true;
}

void CNavigation::Update(CGameObject* _pObj, float _fSpeed)
{
	m_pObj = _pObj;

	if (false == m_bMove || nullptr == m_pNavMesh)
		return;

	Vec3 vPos = m_pObj->Transform()->GetLocalPos();
	Vec3 vTerrainScale = m_pTerrainObj->Transform()->GetLocalScale();
	Vec3 vGoal = {};

	if (m_iPathIdx < (int)m_vecPath.size() - 1)
	{
		Vec3 vLeft = m_vecPath[m_iPathIdx].vCenter * vTerrainScale;
		Vec3 vRight = m_vecPath[m_iPathIdx + 1].vCenter * vTerrainScale;

		vGoal = (vLeft + vRight) * 0.5;
		Vec3 vDir = vGoal - vPos;
		float fDist = vDir.Distance();
		vDir.Normalize();

		vPos += vDir * _fSpeed * CTimeMgr::GetInst()->DeltaTime();
		m_pObj->Transform()->SetLocalPos(vPos);

		if (fDist < 1)
			m_iPathIdx += 1;
	}

	else if (m_iPathIdx == m_vecPath.size() - 1)
	{
		vGoal = m_vGoalPos;
		Vec3 vDir = vGoal - vPos;
		float fDist = vDir.Distance();
		vDir.Normalize();

		vPos += vDir * _fSpeed * CTimeMgr::GetInst()->DeltaTime();
		m_pObj->Transform()->SetLocalPos(vPos);

		if (fDist < 1)
		{
			ResetPath();
		}
	}
	
}

void CNavigation::ResetPath()
{
	m_vecPath.clear();
	m_iStart = -1;
	m_iGoal = -1;

	m_bMove = false;
	m_iPathIdx = 0;
}

