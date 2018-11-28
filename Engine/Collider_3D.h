#pragma once
#include "Collider.h"

class CCollider_3D :
	public CCollider
{
public:
	virtual void Awake();
	virtual int Update() { return 0; }
	virtual void Render();

public:
	virtual bool Is2DCollider() { return false; }
	virtual bool Is3DCollider() { return true; }


public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CCollider_3D);

public:
	CCollider_3D();
	virtual ~CCollider_3D();
};

