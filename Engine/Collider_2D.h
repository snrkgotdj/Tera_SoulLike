#pragma once
#include "Collider.h"

class CCollider_2D :
	public CCollider
{
public:	
	virtual void Awake();
	virtual int Update() { return 0; }
	virtual void Render();

public:
	virtual bool Is2DCollider() { return true; }
	virtual bool Is3DCollider() { return false; }
	
public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CCollider_2D);

public:
	CCollider_2D();
	virtual ~CCollider_2D();
};

