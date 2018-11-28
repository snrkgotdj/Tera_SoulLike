#pragma once
#include "Component.h"
class CAnimation;

struct tDescInfo;

struct tAnimPair
{
	wstring			strKey;
	CAnimation*		pAnim;
};

class CAnimator_2D :
	public CComponent
{
private:
	map<wstring, CAnimation*>	m_mapAnim;
	CAnimation*					m_pCurAnim;
	bool						m_bRepeat;
	bool						m_bStop;

public:
	virtual int Update() { return 0; }
	virtual int LateUpdate();
	
public:	
	void LoadAnimation_2D(const tDescInfo* _pInfo, const wstring& _strFolderPath);
	void PlayAnimation(const wstring& _strKey, bool _bRepeat = true, UINT _iStartFrameIdx = 0);	
	void PlayAnimationReverse(const wstring& _strKey, bool _bRepeat = true);
	void UpdateData();
	static void Disable();
	void GetAnimations(vector<tAnimPair>& _vecOut);
	CAnimation* GetCurAnim() const { return m_pCurAnim; }
	bool IsRepeat() { return m_bRepeat; }
	void SetRepeat(bool _bTrue) { m_bRepeat = _bTrue; }
	bool IsAnimFinish();
	void SetTerm(const wstring& _strKey, float _fTerm);
	void SetCurTerm(float _fTerm);
	float GetCurAnimPrecent();
	void Stop() { m_bStop = true; }
	void Play() { m_bStop = false; }

private:
	CAnimation* FindAnimation(const wstring& _strKey);

	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CAnimator_2D);

public:
	CAnimator_2D();
	CAnimator_2D(const CAnimator_2D& _other);
	virtual ~CAnimator_2D();
};

