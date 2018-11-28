#pragma once
#include "Res.h"

class CGameObject;

class CPrefab :
	public CRes
{
private:
	CGameObject*		m_pObj;
	wstring				m_strLayer;

public:
	CGameObject*	Instantiate();
	CGameObject*	Gameobject() { return m_pObj; }
	const wstring& GetTag();
	void SetLayerName(const wstring& _strLayer) { m_strLayer = _strLayer; }
	const wstring& GetLayerName() { return m_strLayer; }

public:
	CPrefab(CGameObject* _pObj);
	virtual ~CPrefab();
};

