#include "Layer.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"

#include "Scene.h"
#include "SceneMgr.h"

#include "Mesh.h"

CLayer::CLayer()
	: m_bZOrder(false)
	, m_bDestroy(false)
{
}

CLayer::~CLayer()
{
	Safe_Delete_List(m_listObj);
}

void CLayer::Awake()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		(*iter)->Awake();
	}
}

void CLayer::Start()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		(*iter)->Start();
	}
}

int CLayer::Update()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		(*iter)->Update();
	}
	return 0;
}

int CLayer::LateUpdate()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		(*iter)->LateUpdate();
	}
	return 0;
}

int CLayer::DelayUpdate()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		(*iter)->DelayUpdate();
	}
	return 0;
}

int CLayer::FinalUpdate()
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end();)
	{
		if ((*iter)->IsDestroy())
		{
			DestroyObject(*iter);
			iter = m_listParentObj.erase(iter);
		}
		else
		{
			(*iter)->FinalUpdate();
			++iter;
		}
	}
	return 0;
}

void CLayer::RemoveParentObj(CGameObject * _pObj)
{
	list<CGameObject*>::iterator iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		if (*iter == _pObj)
		{
			m_listParentObj.erase(iter);
			return;
		}
	}
}

void CLayer::RemoveObj(CGameObject * _pObj)
{
	list<CGameObject*>::iterator iter = m_listObj.begin();
	for (; iter != m_listObj.end(); ++iter)
	{
		if (*iter == _pObj)
		{
			m_listObj.erase(iter);
			break;
		}
	}

	iter = m_listParentObj.begin();
	for (; iter != m_listParentObj.end(); ++iter)
	{
		if (*iter == _pObj)
		{
			m_listParentObj.erase(iter);
			return;
		}
	}
}

void CLayer::DestroyObject(CGameObject * _pObj)
{
#ifdef _TOOL
	CSceneMgr::GetInst()->SetUpdateCheck(true);
#endif

	list<CGameObject*>::iterator iterChild = _pObj->m_listChildObj.begin();
	for (; iterChild != _pObj->m_listChildObj.end(); ++iterChild)
	{
		CLayer* pChildLayer = CSceneMgr::GetInst()->GetCurScene()->FindLayer((*iterChild)->GetLayerName());
		pChildLayer->DestroyObject(*iterChild);
	}

	list<CGameObject*>::iterator iter = m_listObj.begin();
	for (; iter != m_listObj.end(); ++iter)
	{
		if (*iter == _pObj)
		{
			m_listObj.erase(iter);
			break;
		}
	}

	delete _pObj;
}

void CLayer::AddGameObject(CGameObject * _pObj, bool _bAll)
{
	if (this == nullptr)
	{
		MessageBox(nullptr, L"추가하고자하는 대상 레이어가 없습니다.", L"AddGameobj 에러", MB_OK);
		return;
	}

#ifdef _TOOL
	if (NULL != _pObj)
	{
		CSceneMgr::GetInst()->SetUpdateCheck(true);
	}
#endif

	// _pObj 가 최상위 부모인 경우
	if (!_pObj->HasParent() && _pObj->GetLayerName() != m_strLayerName)
	{	
		m_listParentObj.push_back(_pObj);
	}

	list<CGameObject*> listSource;
	if (_bAll)
	{
		list<CGameObject*> queue;
		queue.push_back(_pObj);

		list<CGameObject*>::iterator iter = queue.begin();
		for (; iter != queue.end();)
		{
			queue.insert(queue.end(), (*iter)->GetChildList().begin(), (*iter)->GetChildList().end());

			if((*iter)->GetLayerName() != m_strLayerName)
				listSource.push_back(*iter);

			iter = queue.erase(iter);
		}
	}	
	else
	{
		if(_pObj->GetLayerName() != m_strLayerName)
			listSource.push_back(_pObj);
	}

	m_listObj.insert(m_listObj.end(), listSource.begin(), listSource.end());

	// Object 가 이미 이전에 다른 레이어에 들어있었다면,
	// 해당 레이어에서 오브젝트를 삭제해야한다.
	list<CGameObject*>::iterator iter = listSource.begin();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	for (; iter != listSource.end(); ++iter)
	{
		CLayer* pLayer = pCurScene->FindLayer((*iter)->GetLayerName());
		if (NULL != pLayer)
			pLayer->RemoveObj((*iter));
		else
		{
			// Light Component 체크
				if ((*iter)->Light())
				{
					CRenderMgr::GetInst()->AddLightObject(*iter);
				}
				else if ((*iter)->Text())
				{
					CRenderMgr::GetInst()->AddTextObject(*iter);
				}
		}

		(*iter)->SetLayerName(m_strLayerName);
	}
}

CGameObject * CLayer::FindObjectByTag(const wstring & _strTag)
{
	list<CGameObject*>::iterator iter = m_listObj.begin();

	for (; iter != m_listObj.end(); ++iter)
	{
		if ((*iter)->GetTag() == _strTag)
			return *iter;
	}

	return NULL;
}

CGameObject * CLayer::FindObject(const Vec2 _vWorldPos)
{
	list<CGameObject*>::iterator iter = m_listObj.begin();

	for (; iter != m_listObj.end(); ++iter)
	{
		if ((*iter)->IsMouseOn(_vWorldPos))
		{
			return *iter;
		}
	}

	return nullptr;
}

CGameObject * CLayer::FindObjectByRay(const tRay & _tRay)
{
	CGameObject* pPickingObj = NULL;
	float fDist = 0.f;
	float fMinDist = 100000.f;

	list<CGameObject*>::iterator iter = m_listObj.begin();
	for (; iter != m_listObj.end(); ++iter)
	{
		if (isnan(_tRay.vOrigin.x) || isnan(_tRay.vOrigin.y) || isnan(_tRay.vOrigin.z))
			return pPickingObj;

		// 충돌 체크를 하기위한 버텍스, 인덱스 버퍼를 가져온다.
		CMesh* pMesh = (*iter)->MeshRender()->GetMesh();
		if (NULL == pMesh)
			return pPickingObj;

		VTX* pVtx = pMesh->GetVtxSysMem();
		WORD* pIdx = (WORD*)pMesh->GetIdxSysMem();

		UINT iPatchCount = pMesh->GetIdxCount() / 3;
		UINT i = 0;

		Matrix matWorld = (*iter)->Transform()->GetWorldMat();

		for (; i < iPatchCount; ++i)
		{
			if (Intersects(_tRay.vOrigin.Convert(), _tRay.vDir.Convert()
				, XMVector3TransformCoord(pVtx[pIdx[i * 3]].vPos.Convert(), matWorld)
				, XMVector3TransformCoord(pVtx[pIdx[i * 3 + 1]].vPos.Convert(), matWorld)
				, XMVector3TransformCoord(pVtx[pIdx[i * 3 + 2]].vPos.Convert(), matWorld)
				, fDist))
			{
				if (fMinDist >= fDist)
				{
					fMinDist = fDist;
					pPickingObj = (*iter);
				}
			}
		}
	}

	return pPickingObj;
}

void CLayer::AddParentObject(CGameObject * _pParentObj)
{
	if(!_pParentObj->HasParent())
		m_listParentObj.push_back(_pParentObj);
}

void CLayer::DestroyObjAll()
{
	list<CGameObject*>::iterator iter = m_listObj.begin();
	for (; iter != m_listObj.end(); ++iter)
	{
		(*iter)->Destroy();
	}
}
