#include "Script.h"


#include "Scene.h"
#include "SceneMgr.h"
#include "Layer.h"

CScript::CScript()
	: m_ID(SCRIPT_END)
{
}

CScript::~CScript()
{
}

CGameObject* CScript::Instantiate(CPrefab * _pPrefab, const Vec3 & _vPos
	, const wstring & _strLayerName)
{
	if (nullptr == _pPrefab)
	{
		MessageBox(nullptr, L"�ش� �������� �����ϴ�", L"Instantiate ����", MB_OK);
		return nullptr;
	}

	CGameObject* pCloneObj = _pPrefab->Instantiate();

	pCloneObj->Transform()->SetLocalPos(_vPos);

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(_strLayerName);
	pLayer->AddGameObject(pCloneObj);

	pCloneObj->Awake();
	pCloneObj->Start();

	return pCloneObj;
}

void CScript::Save(FILE* _pFile)
{
	fwrite(&m_ID, 1, sizeof(UINT), _pFile);


}

void CScript::Load(FILE* _pFile)
{
	fread(&m_ID, 1, sizeof(UINT), _pFile);
}