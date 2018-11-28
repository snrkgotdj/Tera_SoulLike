#include "Prefab.h"

#include "GameObject.h"

CPrefab::CPrefab(CGameObject * _pObj)
	: m_pObj(_pObj)
{
}

CPrefab::~CPrefab()
{
	SAFE_DELETE(m_pObj);
}

CGameObject * CPrefab::Instantiate()
{
	return m_pObj->Clone();
}

const wstring & CPrefab::GetTag()
{
	return m_pObj->GetTag();
}