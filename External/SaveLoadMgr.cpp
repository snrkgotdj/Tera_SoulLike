#include "stdafx.h"
#include "SaveLoadMgr.h"

#include "MgrScript\ScriptMgr.h"

#include "RenderMgr.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"
#include "GameObject.h"
#include "Component.h"
#include "Script.h"
#include "prefab.h"

#include "Transform.h"
#include "MeshRender.h"
#include "Collider_2D.h"
#include "Collider_3D.h"
#include "Animator_2D.h"
#include "Animator_3D.h"
#include "Camera.h"
#include "Light.h"
#include "Text.h"
#include "Terrain.h"
#include "Trail.h"

CSaveLoadMgr::CSaveLoadMgr()
{
}

CSaveLoadMgr::~CSaveLoadMgr()
{
}

//=========
// Save
//=========
void CSaveLoadMgr::SaveScene(const wstring & _strFilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");
	
	wstring strFileName = CPathMgr::GetFileName(_strFilePath.c_str());

	if (NULL == pFile)
		assert(NULL);
	
	// Resource 저장
	CResMgr::GetInst()->SaveResource(pFile);

	// Prefab 저장
	map<wstring, CResPtr<CPrefab>>& mapPrefab = CResMgr::GetInst()->GetPrefabMap();
	UINT iCount = mapPrefab.size();
	fwrite(&iCount, sizeof(UINT), 1, pFile);

	for (auto& pair : mapPrefab)
	{
		SaveResourceKey((CRes*)pair.second, pFile);
		SaveWString(pair.second->GetLayerName(), pFile);
		SaveGameObject(pair.second->Gameobject(), pFile, strFileName);
	}
	
	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();

	// Layer Count
	iCount = pScene->GetLayerCount();
	fwrite(&iCount, sizeof(UINT), 1, pFile);

	const vector<CLayer*>& vecLayer = pScene->GetVecLayer();
	for (UINT i = 0; i < vecLayer.size(); ++i)
	{
		if(NULL != vecLayer[i])
			SaveLayer(vecLayer[i], pFile, strFileName);
	}

	fclose(pFile);
}


void CSaveLoadMgr::SaveLayer(CLayer * _pLayer, FILE * _pFile, const wstring& _strName)
{
	// Layer Idx
	UINT iLayerIdx = _pLayer->GetLayerIdx();
	fwrite(&iLayerIdx, sizeof(UINT), 1, _pFile);
	
	// Layer Name
	SaveWString(_pLayer->GetLayerName(), _pFile);

	// Parent Object Count
	list<CGameObject*>& listParent = _pLayer->GetParentList();
	UINT iObjectCount = listParent.size();

	fwrite(&iObjectCount, sizeof(UINT), 1, _pFile);
	
	list<CGameObject*>::iterator iter = listParent.begin();
	for (; iter != listParent.end(); ++iter)
	{

		SaveGameObject((*iter), _pFile, _strName);
	}
}


void CSaveLoadMgr::SaveGameObject(CGameObject * _pObj, FILE * _pFile, const wstring& _strName)
{
	// Tag
	SaveWString(_pObj->GetTag(), _pFile);
	
	// Parts Idx
	int iIdx = _pObj->GetPartsIdx();
	fwrite(&iIdx, sizeof(int), 1, _pFile);

	// Component
	UINT i = (UINT)COMPONENT_TYPE::TRANSFORM;
	for (i; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		CComponent* pCom = _pObj->GetComponent((COMPONENT_TYPE)i);
		if (NULL == pCom)
			continue;

		//fwrite(&i, sizeof(UINT), 1, _pFile);
		pCom->Save(_pFile);
	}

	// COMPONENT_TYPE::END; 게임오브젝트 컴포넌트 마감
	fwrite(&i, sizeof(UINT), 1, _pFile);

	// Script Count
	map<SCRIPT_ID, CScript*>& listScript = _pObj->GetScriptList();
	UINT iScriptCount = listScript.size();
	fwrite(&iScriptCount, sizeof(UINT), 1, _pFile);

	// Script 
	map<SCRIPT_ID, CScript*>::iterator iter = listScript.begin();
	for (iter; iter != listScript.end(); ++iter)
	{
		// Script 이름 저장
		SaveWString(CScriptMgr::GetScriptName(iter->second), _pFile);
		iter->second->Save(_pFile);
	}

	// Child Count
	list<CGameObject*>& listChild = _pObj->GetChildList();
	UINT iChildCount = listChild.size();
	fwrite(&iChildCount, sizeof(UINT), 1, _pFile);

	// Child Object
	list<CGameObject*>::iterator childIter = listChild.begin();
	for (; childIter != listChild.end(); ++childIter)
	{
		SaveGameObject(*childIter, _pFile, _strName);
	}
}


//=========
// Load
//=========
void CSaveLoadMgr::LoadScene(const wstring & _strFilePath)
{
	// RenderMgr 초기화
	// 이전 Scene 에서의 참조 정보를 초기화
	CRenderMgr::GetInst()->Reset();
	CResMgr::GetInst()->ResetUI();

	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	wstring strFileName = CPathMgr::GetFileName(_strFilePath.c_str());

	if (NULL == pFile)
		assert(NULL);

	// Resource Load
	CResMgr::GetInst()->LoadResource(pFile);

	// Prefab Load	
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		wstring strKey = LoadResourceKey(pFile);
		wstring strLayerName = LoadWString(pFile);
		CGameObject* pObj = LoadGameObject(pFile, strFileName);

		if (RET_FAILED == CResMgr::GetInst()->AddPrefab(strKey, strLayerName, pObj))
		{
			delete pObj;
		}
	}

	CScene* pScene = new CScene;

	// Layer Count	
	fread(&iCount, sizeof(UINT), 1, pFile);
		
	for (UINT i = 0; i < iCount; ++i)
	{
		CLayer* pLayer = LoadLayer(pFile, strFileName);
		pScene->AddLayer(pLayer);
	}

	CSceneMgr::GetInst()->ChangeScene(pScene);
	fclose(pFile);
}

bool CSaveLoadMgr::SaveGameObject(const wstring & _strFilePath, CGameObject * _pObj)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

	if (NULL == pFile)
	{
		MessageBox(NULL, L"세이브 실패", L"세이브 경로 오류", MB_OK);
		return false;
	}

	CResMgr::GetInst()->SaveResource(pFile);

	wstring strFileName = CPathMgr::GetFileName(_strFilePath.c_str());

	SaveGameObject(_pObj, pFile, strFileName);

	fclose(pFile);

	return 0;
}

CGameObject* CSaveLoadMgr::LoadGameObject(const wstring & _strFilePath)
{
	FILE* pFile = NULL;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	if (NULL == pFile)
	{
		MessageBox(NULL, L"로드 실패", L"로드 경로 오류", MB_OK);
		return nullptr;
	}

	wstring strFileName = CPathMgr::GetFileName(_strFilePath.c_str());

	CResMgr::GetInst()->LoadResource(pFile);

	CGameObject* pObj = LoadGameObject(pFile, strFileName);

	CSceneMgr::GetInst()->GetCurScene()->FindLayer(L"Default")->AddGameObject(pObj);

	fclose(pFile);

	return pObj;
}


CLayer * CSaveLoadMgr::LoadLayer(FILE * _pFile, const wstring& _strName)
{
	CLayer* pLayer = new CLayer;

	// Read layer Index
	UINT iLayerIdx = 0;
	fread(&iLayerIdx, sizeof(UINT), 1, _pFile);
	pLayer->SetLayerIdx(iLayerIdx);
		
	// Read Layer Name
	pLayer->SetLayerName(LoadWString(_pFile));

	// Parent Object Count	
	UINT iObjectCount = 0;
	fread(&iObjectCount, sizeof(UINT), 1, _pFile);
		
	for (UINT i = 0; i < iObjectCount; ++i)
	{

		CGameObject* pObj = LoadGameObject(_pFile, _strName);
		pLayer->AddGameObject(pObj);
	}

	return pLayer;
}


CGameObject * CSaveLoadMgr::LoadGameObject(FILE * _pFile, const wstring& _strName)
{
	CGameObject* pObj = new CGameObject;

	// Tag
	pObj->SetTag(LoadWString(_pFile));

	// Parts Idx
	int iIdx = -1;
	fread(&iIdx, sizeof(int), 1, _pFile);
	pObj->SetPartsIdx(iIdx);

	// Component
	UINT iType = 0;
	CComponent* pComponent = NULL;

	while (iType != (UINT)COMPONENT_TYPE::END)
	{
		fread(&iType, sizeof(UINT), 1, _pFile);


		switch (iType)
		{
		case (UINT)COMPONENT_TYPE::TRANSFORM:
			pComponent = new CTransform;
			pObj->AddComponent<CTransform>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::MESHRENDER:
			pComponent = new CMeshRender;
			pObj->AddComponent<CMeshRender>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::COLLIDER_2D:
			pComponent = new CCollider_2D;
			pObj->AddComponent<CCollider>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::COLLIDER_3D:
			pComponent = new CCollider_3D;
			pObj->AddComponent<CCollider>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::ANIMATOR_2D:
			pComponent = new CAnimator_2D;
			pObj->AddComponent<CAnimator_2D>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::ANIMATOR_3D:
			pComponent = new CAnimator_3D;
			pObj->AddComponent<CAnimator_3D>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::CAMERA:
			pComponent = new CCamera;
			pObj->AddComponent<CCamera>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::LIGHT:
			pComponent = new CLight;
			pObj->AddComponent<CLight>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::TEXT:
			pComponent = new CText;
			pObj->AddComponent<CText>(pComponent);
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::TERRAIN:
			pComponent = new CTerrain;
			pObj->AddComponent<CTerrain>(pComponent);
			((CTerrain*)pComponent)->LoadTileTexture();
			pComponent->Load(_pFile);
			break;
		case (UINT)COMPONENT_TYPE::TRAIL:
			pComponent = new CTrail;
			pObj->AddComponent<CTrail>(pComponent);
			pComponent->Load(_pFile);
			break;
		}
	}

	// Script Count
	UINT iCount = 0;
	wstring strScriptName;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	// Script 	
	for (UINT i = 0; i < iCount; ++i)
	{
		// Script 이름 저장
		strScriptName = LoadWString(_pFile);
		CScript* pScript = CScriptMgr::GetScript(strScriptName);
		pScript->Load(_pFile);

		pObj->AddScript(pScript->GetScriptID(), pScript);
		//pObj->AddComponent<CScript>(pScript);
	}

	// Child Count
	fread(&iCount, sizeof(UINT), 1, _pFile);

	// Child Object
	for (UINT i = 0; i < iCount; ++i)
	{
		CGameObject* pChildObj = LoadGameObject(_pFile, _strName);
		pObj->AddChildObj(pChildObj);

		if (0 <= pChildObj->GetPartsIdx())
			pObj->Animator_3D()->SetPartsObjAtSave(pChildObj->GetPartsIdx(), pChildObj);
	}

	return pObj;
}