#pragma once

#include "global.h"


class CAnimator_2D;

class CAnimation
{
protected:
	CAnimator_2D*		m_pAnimator;
	wstring				m_strKey;
	bool				m_bActive;

	int					m_iCurFrame;
	float				m_fAccTime;
	bool				m_b2DAnim;

	bool				m_bReverse;

public:
	virtual int LateUpdate() = 0;
	virtual void UpdateData() = 0;

public:
	void SetKey(const wstring& _strKey) { m_strKey = _strKey; }
	const wstring& GetKey() { return m_strKey; }
	void SetAnimator(CAnimator_2D* _pAnimator) { m_pAnimator = _pAnimator; }
	void SetFrameIdx(UINT _iFrameIdx) { m_iCurFrame = _iFrameIdx; }
	void SetActive(bool _bOn) { m_bActive = _bOn; }
	bool IsActive() { return m_bActive; }
	bool Is2DAnim() { return m_b2DAnim; }
	UINT GetCurFrame() { return m_iCurFrame; }
	void SetReverse(bool _bTrue) { m_bReverse = _bTrue; }
	bool GetReverse() { return m_bReverse; }

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	virtual CAnimation* Clone() = 0;

public:
	CAnimation();
	virtual ~CAnimation();
};

