#pragma once
#include "Animation.h"

#include "ResPtr.h"

class CTexture;

struct tFrameInfo
{
	CResPtr<CTexture> pTex;
	Vec2			  vLeftTop;
	float			  fWidth;
	float			  fHeight;
	float			  fTerm;
};

struct tDescInfo
{
	wstring strTexName;
	wstring strAnimName;
	Vec2    vLeftTop;
	Vec2    vSize;
	UINT    iContinue;
	UINT    iFrameCountX;
	UINT    iFrameCountY;
};

class CAnimation_2D :
	public CAnimation
{
private:
	vector<tFrameInfo>  m_vecFrame;

public:
	virtual int LateUpdate();
	virtual void UpdateData();	

private:
	void AddFrame(const tFrameInfo& _tFrame) { m_vecFrame.push_back(_tFrame); }

public:
	static CAnimation_2D* FromDescToAnimation(const tDescInfo& _tDesc, const wstring& _strFolderPath);
	void SetTerm(float _fTerm);
	void SetCurFrame(int _iCurFrame) { m_iCurFrame = _iCurFrame; }
	UINT GetFrameSize() { return (UINT)m_vecFrame.size(); }
	CTexture* GetCurTex() { return m_vecFrame[m_iCurFrame].pTex; }

public:
	void Save(FILE* _pFile);
	void Load(FILE* _pFile);
	CLONE(CAnimation_2D);

public:
	CAnimation_2D();
	virtual ~CAnimation_2D();
};

