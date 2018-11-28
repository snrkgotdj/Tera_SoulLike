#pragma once
#include "Script.h"

#include "SceneMgr.h"
#include "Scene.h"
#include "Layer.h"

template<typename T>
class CSTScript :
	public CScript
{
protected:
	static T*  g_pThis;
	
public:
	static T* GetInst();

public:
	CSTScript() {};
	virtual ~CSTScript() { g_pThis = NULL; }
};

template<typename T>
T* CSTScript<T>::g_pThis = NULL;

template<typename T>
T * CSTScript<T>::GetInst()
{
	if (NULL == g_pThis)
	{
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		CLayer* pLayer = pCurScene->FindLayer(L"Default");
		CGameObject* pObj = pLayer->FindObjectByTag(L"MgrObject");

		if (NULL == pObj)
		{
			pObj = new CGameObject;
			pObj->AddComponent<CTransform>(new CTransform);
			pObj->SetTag(L"MgrObject");
			pLayer->AddGameObject(pObj);
			
			g_pThis = new T;
			pObj->AddComponent<CScript>(g_pThis);
		}
		else
		{
			T* pScript = (T*)pObj->GetComponent<T>();
			if (NULL != pScript)
			{
				g_pThis = pScript;
			}
			else
			{
				g_pThis = new T;
				pObj->AddComponent<CScript>(g_pThis);
			}
		}
	}

	return g_pThis;
}

