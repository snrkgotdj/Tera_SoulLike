#pragma once
#include "Res.h"
#include "fmod/fmod.h"
#include "fmod/fmod.hpp"
#include "fmod/fmod_codec.h"

#ifdef WIN64
	#ifdef _DEBUG
	#pragma comment(lib, "fmod64_vc.lib")
	#else
	#pragma comment(lib, "fmodL64_vc.lib")
	#endif	
#else
	#ifdef _DEBUG
	#pragma comment(lib, "fmod_vc.lib")
	#else
	#pragma comment(lib, "fmodL_vc.lib")
	#endif
#endif

class CTransform;

class CSound :
	public CRes
{
public:
	static FMOD::System*	g_pSystem;

private:
	FMOD::Sound*	m_pSound;

private:
	static CSound* Create(const wstring& _strFullPath, bool _bIs3D = false);

public:
	static int Update();

public:
	void Release() {if (NULL != m_pSound) m_pSound->release();};
	FMOD::Sound* GetSound() { return m_pSound; }

private:
	CLONE(CSound);

public:
	CSound();
	virtual ~CSound();

	friend class CResMgr;
};

