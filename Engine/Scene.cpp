#include "Scene.h"

#include "Layer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"

#include "SceneMgr.h"

CScene::CScene()	
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_setLayerIdx.insert(i);
	}

	m_vecLayer.resize(MAX_LAYER);
}

CScene::~CScene()
{
	Safe_Delete_Vec(m_vecLayer);
}

void CScene::Awake()
{
	CGameObject* pObj = CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Player")->FindObjectByTag(L"Player");
	CSceneMgr::GetInst()->SetPlayer(pObj);
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
			m_vecLayer[i]->Awake();
	}
}

void CScene::Start()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
			m_vecLayer[i]->Start();
	}
}

int CScene::Update()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
			m_vecLayer[i]->Update();
	}
	return 0;
}

int CScene::LateUpdate()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
			m_vecLayer[i]->LateUpdate();
	}
	return 0;
}

int CScene::DelayUpdate()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
			m_vecLayer[i]->DelayUpdate();
	}
	return 0;
}

int CScene::FinalUpdate()
{
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		if (NULL != m_vecLayer[i])
		{
			if (m_vecLayer[i]->IsDestroy())
				DeleteLayer(m_vecLayer[i]->GetLayerName());
			else
				m_vecLayer[i]->FinalUpdate();
		}
	}
	return 0;
}

int CScene::AddLayer(const wstring & _strLayerName)
{
	CLayer* pLayer = FindLayer(_strLayerName);

	if (NULL != pLayer)
		return RET_FAILED;
		
	pLayer = new CLayer;
	UINT iLayerIdx = GetLayerIdx();

	pLayer->SetLayerIdx(iLayerIdx);
	pLayer->SetLayerName(_strLayerName);

	m_mapLayer.insert(make_pair(_strLayerName, pLayer));
	m_vecLayer[iLayerIdx] = pLayer;

	return RET_SUCCESS;
}

int CScene::AddLayer(CLayer * _pLayer)
{
	if (FindLayer(_pLayer->GetLayerName()))
		assert(NULL);

	UINT iLayerIdx = _pLayer->GetLayerIdx();

	if (NULL != m_vecLayer[iLayerIdx])
		return RET_FAILED;
	
	set<UINT>::iterator iter = m_setLayerIdx.find(iLayerIdx);
	if(m_setLayerIdx.end() != iter)
		m_setLayerIdx.erase(iter);
	
	m_mapLayer.insert(make_pair(_pLayer->GetLayerName(), _pLayer));
	m_vecLayer[iLayerIdx] = _pLayer;

	return RET_SUCCESS;
}

CLayer * CScene::FindLayer(const wstring & _strLayerName)
{
	map<wstring, CLayer*>::iterator iter = m_mapLayer.find(_strLayerName);
	if (iter == m_mapLayer.end())
		return NULL;

	return iter->second;
}

CLayer * CScene::FindLayer(UINT _iIdx)
{
	return m_vecLayer[_iIdx];
}

UINT CScene::GetLayerIdx()
{
	if (m_setLayerIdx.empty())
		assert(NULL);

	UINT iLayerIdx = *(m_setLayerIdx.begin());
	m_setLayerIdx.erase(m_setLayerIdx.begin());
	return iLayerIdx;
}

void CScene::FindPlayer()
{
	
}

CGameObject * CScene::FindObject(Vec2 _vWindowPos)
{
	Vec2 vWorldPos = Get2DWorldPosFromWindowPos(_vWindowPos);

	for (UINT i = 0; i < m_vecLayer.size(); ++i)
	{
		if (NULL == m_vecLayer[i])
			continue;

		CGameObject* pTarget = m_vecLayer[i]->FindObject(vWorldPos);
		if (NULL != pTarget)
			return pTarget;
	}

	return nullptr;
}

void CScene::GetAllParentObjects(list<CGameObject*>& _listOut)
{
	for (UINT i = 0; i < m_vecLayer.size(); ++i)
	{
		if (NULL == m_vecLayer[i] || m_vecLayer[i]->IsDestroy())
			continue;

		list<CGameObject*>& listParent = m_vecLayer[i]->GetParentList();
		_listOut.insert(_listOut.end(), listParent.begin(), listParent.end());
	}
}

void CScene::DeleteLayer(const wstring & _strKey)
{
	map<wstring, CLayer*>::iterator iter = m_mapLayer.find(_strKey);	
	if (iter == m_mapLayer.end())
		return;

	m_setLayerIdx.insert(iter->second->GetLayerIdx());
	m_vecLayer[iter->second->GetLayerIdx()] = NULL;
	SAFE_DELETE(iter->second);
	m_mapLayer.erase(iter);
}

void CScene::DestoryLayer(UINT _iIdx)
{
	if (NULL != m_vecLayer[_iIdx])
		m_vecLayer[_iIdx]->Destroy();
}

Vec2 CScene::Get2DWorldPosFromWindowPos(Vec2 _vWindowPos)
{
	Vec2 vResolution = CRenderMgr::GetInst()->GetWindowResolution();

	Vec2 vWorldPos;
	vWorldPos.x = _vWindowPos.x - (vResolution.x / 2.f);
	vWorldPos.y = (vResolution.y / 2.f) - _vWindowPos.y;

	CLayer* pCamLayer = FindLayer(L"Camera");
	CGameObject* pCamObj = pCamLayer->FindObjectByTag(L"MainCamera");
	float fScale = pCamObj->Camera()->GetScale();
	
	vWorldPos = Vec2(vWorldPos.x * fScale + pCamObj->Transform()->GetWorldPos().x
		, vWorldPos.y * fScale + pCamObj->Transform()->GetWorldPos().y);

	return vWorldPos;
}

#include "ResMgr.h"
#include "MeshRender.h"
CGameObject * CScene::GetUIObject()
{
	CLayer* pLayer = FindLayer(L"UI");
	CGameObject* pUI = FindLayer(L"UI")->FindObjectByTag(L"UI");
	if (NULL == pUI)
	{
		pUI = new CGameObject;
		pUI->SetTag(L"UI");
		pUI->AddComponent<CTransform>(new CTransform);
		pUI->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
		pLayer->AddGameObject(pUI);

		CGameObject* pCanvas = new CGameObject;
		pCanvas->SetTag(L"Canvas");
		pCanvas->AddComponent<CTransform>(new CTransform);
		pCanvas->AddComponent<CMeshRender>(new CMeshRender);

		pCanvas->Transform()->SetLocalScale(Vec3(WINSIZE_X, WINSIZE_Y, 1.f));

		CMesh* pMesh = (CMesh*)CResMgr::GetInst()->Load<CMesh>(L"ColliderRectMesh");
		pCanvas->MeshRender()->SetMesh(pMesh);
		pCanvas->MeshRender()->SetMaterial(CResMgr::GetInst()->FindMaterial(L"DefaultMtrl"));

		pLayer = FindLayer(L"Tool");
		pLayer->AddGameObject(pCanvas);
		pUI->AddChildObj(pCanvas);

		CGameObject* pUICam = new CGameObject;
		pUICam->SetTag(L"UICamera");
		pUICam->AddComponent<CTransform>(new CTransform);
		pUICam->AddComponent<CCamera>(new CCamera);

		pUICam->Transform()->SetLocalPos(Vec3(0.f, 0.f, -1.f));
		pUICam->Camera()->LayerRenderCheck(L"UI");
		pUICam->Camera()->SetPerspective(false);	
		pUICam->Camera()->SetWidth(WINSIZE_X);
		pUICam->Camera()->SetHeight(WINSIZE_Y);

		pLayer = FindLayer(L"Camera");
		pLayer->AddGameObject(pUICam);
		CSceneMgr::GetInst()->SetCamera(CAMERA_TYPE::UI, pUICam);
	}

	return pUI;
}
bool CScene::Save(const wstring & _strPath)
{
	return false;
}

bool CScene::Load(const wstring & _strPath)
{
	return false;
}