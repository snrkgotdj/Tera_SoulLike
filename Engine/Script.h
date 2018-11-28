#pragma once
#include "Component.h"

#include "Prefab.h"
#include "GameObject.h"

#include "Transform.h"
#include "MeshRender.h"
#include "Camera.h"
#include "Collider.h"
#include "Animator_2D.h"

#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ResMgr.h"

#include "Material.h"
#include "define_user.h"

class CScript :
	public CComponent
{
protected:
	SCRIPT_ID m_ID;

public:
	void SetScriptID(SCRIPT_ID _ScriptID) {	m_ID = _ScriptID; }
	SCRIPT_ID GetScriptID() { return m_ID; }

public:
	virtual void Awake() {}
	virtual void Start() {}
	virtual int Update() { return 0; }
	virtual int LateUpdate() { return 0; }	

protected:
	float DT() { return CTimeMgr::GetInst()->DeltaTime(); }
	CGameObject* Instantiate(CPrefab* _pPrefab, const Vec3& _vPos, const wstring& _strLayerName = L"Default");

public:
	virtual void OnCollisionEnter(CCollider* _pOther) {};
	virtual void OnCollision(CCollider* _pOther) {};
	virtual void OnCollisionExit(CCollider* _pOther) {};

public:
	virtual CScript* Clone() = 0;
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CScript();
	virtual ~CScript();
};

