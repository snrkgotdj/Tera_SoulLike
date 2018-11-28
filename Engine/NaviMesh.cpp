#include "NaviMesh.h"
#include "NaviCell.h"


CNaviMesh::CNaviMesh()
{
	
}

CNaviMesh::~CNaviMesh()
{
	Safe_Delete_Map(m_mapNaviCell);

}

bool CNaviMesh::AddCell(Vec3 & vPointA, Vec3 & vPointB, Vec3 & vPointC, UINT _iPatchIdx)
{
	if (_iPatchIdx < 0)
		return false;

	CNaviCell* pNewCell = FindNaviCell(_iPatchIdx);

	if (pNewCell != nullptr)
		return false;

	pNewCell = new CNaviCell;
	pNewCell->init(vPointA, vPointB, vPointC);
	pNewCell->SetPatchIdx(_iPatchIdx);

	m_mapNaviCell.insert(make_pair(_iPatchIdx, pNewCell));

	if (0 == (_iPatchIdx % 2))
	{
		if (0 == (_iPatchIdx % 64))
		{
			LinkNavi(pNewCell, _iPatchIdx + 1, SIDE_AB);
			LinkNavi(pNewCell, _iPatchIdx - (m_iFaceX * 2) + 1, SIDE_BC);
		}
		else
		{
			LinkNavi(pNewCell, _iPatchIdx + 1, SIDE_AB);
			LinkNavi(pNewCell, _iPatchIdx - 1, SIDE_CA);
			LinkNavi(pNewCell, _iPatchIdx - (m_iFaceX * 2) + 1, SIDE_BC);
		}
	}
	else
	{
		if (63 == (_iPatchIdx % 64))
		{
			LinkNavi(pNewCell, _iPatchIdx - 1, SIDE_AB);
			LinkNavi(pNewCell, _iPatchIdx + (m_iFaceX * 2) - 1, SIDE_BC);
		}
		else
		{
			LinkNavi(pNewCell, _iPatchIdx - 1, SIDE_AB);
			LinkNavi(pNewCell, _iPatchIdx + 1, SIDE_CA);
			LinkNavi(pNewCell, _iPatchIdx + (m_iFaceX * 2) - 1, SIDE_BC);
		}
		
	}

	return true;
}

bool CNaviMesh::AddCell(CNaviCell * _pCell)
{
	if (_pCell->GetPatchIdx() < 0)
		return false;

	UINT iPatchIdx = _pCell->GetPatchIdx();

	CNaviCell* pNewCell = FindNaviCell(iPatchIdx);

	if (pNewCell != nullptr)
		return false;

	pNewCell = _pCell;
	m_mapNaviCell.insert(make_pair(iPatchIdx, pNewCell));

	if (0 == (iPatchIdx % 2))
	{
		if (0 == (iPatchIdx % 64))
		{
			LinkNavi(pNewCell, iPatchIdx + 1, SIDE_AB);
			LinkNavi(pNewCell, iPatchIdx - (m_iFaceX * 2) + 1, SIDE_BC);
		}
		else
		{
			LinkNavi(pNewCell, iPatchIdx + 1, SIDE_AB);
			LinkNavi(pNewCell, iPatchIdx - 1, SIDE_CA);
			LinkNavi(pNewCell, iPatchIdx - (m_iFaceX * 2) + 1, SIDE_BC);
		}
	}
	else
	{
		if (63 == (iPatchIdx % 64))
		{
			LinkNavi(pNewCell, iPatchIdx - 1, SIDE_AB);
			LinkNavi(pNewCell, iPatchIdx + (m_iFaceX * 2) - 1, SIDE_BC);
		}
		else
		{
			LinkNavi(pNewCell, iPatchIdx - 1, SIDE_AB);
			LinkNavi(pNewCell, iPatchIdx + 1, SIDE_CA);
			LinkNavi(pNewCell, iPatchIdx + (m_iFaceX * 2) - 1, SIDE_BC);
		}
	}

	return true;
}

bool CNaviMesh::FindPath(UINT _iStartPatch, UINT _iGoalPatch, vector<tPathNode>* _vecPath)
{
	CNaviCell* pStartCell =FindNaviCell(_iStartPatch);

	Vec3 vCenter = pStartCell->GetCenter();
	map<int, tPathNode> Openlist;
	map<int, tPathNode> CloseList;
	vector<tPathNode> vecCloseList;

	// 도착 노드
	CNaviCell* pGoalCell = FindNaviCell(_iGoalPatch);

	if (nullptr == pGoalCell) // 도착노드가 길이 아니다
		return false;

	_vecPath->clear();

	//시작 노드를 클로즈 리스트에 넣는다
	UINT iParentIdx = pStartCell->GetPatchIdx();
	tPathNode tPath = {};
	tPath.iIdx = iParentIdx;
	tPath.vCenter = pStartCell->GetCenter();
	tPath.iParentIdx = -1;
	CloseList.insert(make_pair(tPath.iIdx, tPath));

	tPathNode tStart = tPath;

	//출발노드가 도착노드
	if (_iStartPatch == _iGoalPatch)
	{
		_vecPath->push_back(tStart);
		return true;
	}

	while (true)
	{
		// 인접 노드를 오픈리스트에 넣는다
		for (int i = 0; i < 3; ++i)
		{
			CNaviCell* pCell = pStartCell->GetLinkCell((CELL_SIDE)i);
			if (nullptr == pCell)
				continue;
			
			if (CloseList.find(pCell->GetPatchIdx()) != CloseList.end())
				continue;

			memset(&tPath, 0, sizeof(tPathNode));
			tPathNode tParentNode = CloseList.find(iParentIdx)->second;
			tPath.vCenter = pCell->GetCenter();
			tPath.fGScore = tParentNode.fGScore + ::Distance(tPath.vCenter, tParentNode.vCenter);
			tPath.fHScore = ::Distance(tPath.vCenter, pGoalCell->GetCenter());
			tPath.fFScore = tPath.fGScore + tPath.fHScore;
			tPath.iIdx = pCell->GetPatchIdx();
			tPath.iParentIdx = iParentIdx;
			
			
			map<int, tPathNode>::iterator iter = Openlist.find(tPath.iIdx);
			if (Openlist.end() != iter) //만약 인접 노드가 이미 있다면
			{
				// fGScore를 비교 현재가 더 작으면 삭제 후 인서트
				if (iter->second.fGScore > tPath.fGScore)
				{
					Openlist.erase(iter);
					Openlist.insert(make_pair(tPath.iIdx, tPath));
				}
			}
			else
			{
				Openlist.insert(make_pair(tPath.iIdx, tPath));
			}
		}

		// fScore가 제일 짧은 노드 찾기
		float fFullDist = 0;
		map<int, tPathNode>::iterator Shortiter;
		map<int, tPathNode>::iterator iter = Openlist.begin();
		for (; iter != Openlist.end(); ++iter)
		{
			if (fFullDist == 0)
			{
				fFullDist = iter->second.fFScore;
				Shortiter = iter;
			}

			else
			{
				if (fFullDist > iter->second.fFScore)
				{
					fFullDist = iter->second.fFScore;
					Shortiter = iter;
				}
			}
		}
		
		pStartCell = FindNaviCell(Shortiter->first);	
		iParentIdx = Shortiter->first;
		
		CloseList.insert(make_pair(Shortiter->first, Shortiter->second));// Closelist에 노드 추가
		vecCloseList.push_back(Shortiter->second);
		if (Shortiter->first == _iGoalPatch)// 목표노드 도달했으면 종료
			break;

		Openlist.erase(Shortiter);// Openlist에서 삭제
	}

	vector<tPathNode> tTempVec;
	tTempVec.push_back(vecCloseList[vecCloseList.size() - 1]);
	while (true)
	{
		tPathNode tResult = tTempVec.back();
		tResult = CloseList.find(tResult.iParentIdx)->second;
		tTempVec.push_back(tResult);

		if (tResult.iIdx == _iStartPatch)
			break;
	}
	for (UINT i = 0; i < tTempVec.size(); ++i)
	{
		_vecPath->push_back(tTempVec[tTempVec.size() - i - 1]);
	}

	return true;
}

void CNaviMesh::SetFaceCount(UINT _iFaceX, UINT _iFaceZ)
{
	m_iFaceX = _iFaceX;
	m_iFaceZ = _iFaceZ;
}

CNaviCell * CNaviMesh::FindNaviCell(UINT _iIdx)
{
	if (_iIdx < 0)
		return nullptr;

	map<UINT, CNaviCell*>::iterator iter = m_mapNaviCell.find(_iIdx);

	if (iter == m_mapNaviCell.end())
		return nullptr;

	return iter->second;
}

void CNaviMesh::LinkNavi(CNaviCell* _pNewCell, UINT _iIdx, CELL_SIDE _eLine)
{
	CNaviCell* pLinkNavi = FindNaviCell(_iIdx);

	if (nullptr == pLinkNavi)
		return;

	if (pLinkNavi->GetLineMid(_eLine) != _pNewCell->GetLineMid(_eLine))
	{
		assert(NULL);// 같은 변을 공유하므로 중점이 무조건 같아야한다
	}

	if (nullptr == pLinkNavi->GetLinkCell(_eLine) && nullptr == _pNewCell->GetLinkCell(_eLine))
	{
		pLinkNavi->SetLinkCell(_eLine, _pNewCell);
		_pNewCell->SetLinkCell(_eLine, pLinkNavi);
	}
}

void CNaviMesh::Save(FILE * _pFile)
{
	UINT iSize = m_mapNaviCell.size();
	fwrite(&iSize, sizeof(UINT), 1, _pFile );

	fwrite(&m_iFaceX, sizeof(UINT), 1, _pFile);
	fwrite(&m_iFaceZ, sizeof(UINT), 1, _pFile);

	for (auto& iter : m_mapNaviCell)
	{
		fwrite(&iter.first, sizeof(UINT), 1, _pFile);
		iter.second->Save(_pFile);
	}
}

void CNaviMesh::Load(FILE* _pFile)
{
	UINT iSize = 0;
	fread(&iSize, sizeof(UINT), 1, _pFile);

	fread(&m_iFaceX, sizeof(UINT), 1, _pFile);
	fread(&m_iFaceZ, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < iSize; ++i)
	{
		UINT iIdx = -1;
		fread(&iIdx, sizeof(UINT), 1, _pFile); // PatchIdx

		CNaviCell* pNewCell = new CNaviCell; // Cell
		pNewCell->Load(_pFile);

		AddCell(pNewCell);
	}
}
