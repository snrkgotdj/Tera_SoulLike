#pragma once

#include "define.h"

class CScene;
class CLayer;
class CGameObject;

class CSaveLoadMgr
{
	SINGLE(CSaveLoadMgr);

public:
	void SaveScene(const wstring& _strFilePath);
	void LoadScene(const wstring& _strFilePath);

	bool SaveGameObject(const wstring & _strFilePath, CGameObject * _pObj);
	CGameObject* LoadGameObject(const wstring & _strFilePath);

private:
	void SaveLayer(CLayer* _pLayer, FILE* _pFile, const wstring& _strName);
	CLayer* LoadLayer(FILE* _pFile, const wstring& _strName);

	void SaveGameObject(CGameObject* _pObj, FILE* _pFile, const wstring& _strName);
	CGameObject* LoadGameObject(FILE* _pFile, const wstring& _strName);
};

