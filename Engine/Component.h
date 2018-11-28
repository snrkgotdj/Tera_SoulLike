#pragma once

#include "global.h"

class CGameObject;
class CTransform;
class CMeshRender;
class CCollider;
class CAnimator_2D;
class CAnimator_3D;
class CCamera;
class CLight;
class CTerrain;
class CText;
class CTrail;
class CAudio;

class CComponent
{
private:
	CGameObject*	m_pObj;
	
public:
	virtual void Awake() {};
	virtual void Start() {};
	virtual int Update() = 0;
	virtual int LateUpdate() { return 0; };
	virtual int DelayUpdate() { return 0; }
	virtual int FinalUpdate() { return 0; };
	virtual void Render() {};

private:
	void SetGameObject(CGameObject* _pObj) { m_pObj = _pObj; }

public:
	CGameObject* GetParentObject() const;
	CTransform* Transform();
	CMeshRender* MeshRender();
	CCollider* Collider();
	CAnimator_2D* Animator_2D();
	CAnimator_3D* Animator_3D();
	CCamera* Camera();
	CLight*	Light();
	CTerrain* Terrain();
	CText* Text();
	CTrail* Trail();
	CAudio* Audio();

	virtual void Save(FILE* _pFile) = 0;
	virtual void Load(FILE* _pFile) = 0;

	virtual CComponent* Clone() = 0;

public:
	CGameObject* GetGameObject() { return m_pObj; }

public:
	CComponent();
	virtual ~CComponent();

	friend class CGameObject;
	friend class CSaveLoadMgr;
};
