#pragma once
#include "global.h"

#include "Text.h"

class CTimeMgr
{
	SINGLE(CTimeMgr)

private:
	LARGE_INTEGER	m_llFrequency;
	LARGE_INTEGER	m_llCurCnt;
	LARGE_INTEGER	m_llOldCnt;

	IFW1Factory*		m_pFW1Factory;
	IFW1FontWrapper*	m_pFontWrapper;

	float			m_fOldDeltaTime;
	float			m_fDeltaTime;
	float			m_fAccTime;
	float			m_fFrameLimit;

	float			m_fFPS;	
	float			m_fAccFPS;
	int				m_iAccFrame;
	
	float			m_fTimer;

public:
	void init();
	int Update();
	void render(HDC _dc);

public:
	float GetTimer() { return m_fTimer; }
	float DeltaTime() { return m_fDeltaTime; }
	float GetFPS() { return m_fAccFPS; }
};

