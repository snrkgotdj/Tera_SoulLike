#pragma once
#include "Script.h"

enum class TILE_TYPE
{
	BLOOD,
	CRACK,
	CRACK2,
	END
};

class CTileEffect :
	public CScript
{
private:
	float		m_fLiveTime;
	float		m_fTime;

	TILE_TYPE	m_eType;

public:
	virtual int Update();

public:
	void SetTime(float _fTime) { m_fTime = _fTime; }
	void SetTile(TILE_TYPE _eType);

public:
	CLONE(CTileEffect);

public:
	CTileEffect();
	virtual ~CTileEffect();
};