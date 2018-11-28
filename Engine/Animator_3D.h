#pragma once

#include "Component.h"
#include "FBXLoader.h"

#include "ResPtr.h"

class CTexture;
class CGameObject;

struct tAnimationInfo
{
	float	fStartTime;
	float	fEndTime;
	float	fUpdateTime;

	UINT	iStartFrame;
	UINT	iEndFrame;

	float	fSpeed;

	float	fBenldTime;
};

struct tPARTSOBJ
{
	UINT iBoneIdx;
	CGameObject* pObj;
};

class CAnimator_3D :
	public CComponent
{
private:
	const vector<tMTBone>*				m_pVecBones;
	const vector<tMTAnimClip>*			m_pVecClip;

	vector<Matrix>						m_vecFinalBoneMat;		// 텍스쳐에 전달할 최종 행렬정보
	vector<Matrix>*						m_pVecFinal;
	vector<vector<Matrix>>				m_vecPrevBoneMat;
	Matrix								m_matPrevSpineMat;
	int									m_iFrameCount;			// 30
	int									m_iCurClip;				// 클립 인덱스
	CResPtr<CTexture>					m_pBoneTex;
	CResPtr<CTexture>					m_pPrevBoneTex;

	map<wstring, tAnimationInfo>		m_mapAnim;
	tAnimationInfo						m_tCurAnim;
	wstring								m_strCurKey;
	bool								m_bPlay;
	bool								m_bStop;
	bool								m_bPlayOnce;

	bool								m_bIsAnimFinish;

	vector<tPARTSOBJ>					m_vecPartsObj;
	Matrix								m_matStandard;
	int									m_iFootIdx;
	bool								m_bChangeAnim;

	tAnimationInfo						m_tPrevAnim;
	bool								m_bIsBlening;
	float								m_fBlendTime;
	float								m_fBlendCoolTime;

	bool								m_bBlur;
	int									m_iBlurFrame;
	int									m_iCurFrame;

	float								m_fRSSpeed;
	float								m_fRSTime;
	float								m_fRSCoolTime;
	bool								m_bRS;

public:
	virtual int Update();
	virtual int LateUpdate();
	virtual int DelayUpdate();

public:
	void SetPartsObj(UINT _iBoneIdx, CGameObject* _pObj);
	void SetPartsObjAtSave(UINT _iBoneIdx, CGameObject* _pObj);
	void SetBones(const vector<tMTBone>* _vecBones) { m_pVecBones = _vecBones; m_vecFinalBoneMat.resize(m_pVecBones->size()); }
	void SetAnimClip(const vector<tMTAnimClip>* _vecAnimClip) { m_pVecClip = _vecAnimClip; }
	void SetBoneTex(CTexture* _pBoneTex) { m_pBoneTex = _pBoneTex; }
	void SetPrevBoneTex(CTexture* _pPrevBoneTex) { m_pPrevBoneTex = _pPrevBoneTex; }
	void SetBlur(bool _bBlur, int _bPrevFrame = 1);
	void SetFinalBoneMatrix(vector<Matrix> _Finalmat) { m_vecFinalBoneMat = _Finalmat; }
	void SetFinalBoneMatrixPtr(vector<Matrix>* _Finalmat) { m_pVecFinal = _Finalmat; }
	vector<Matrix>& GetFinalBoneMatrix() { return m_vecFinalBoneMat; }
	void UpdateData();
	void ClearData();

	int AddAnimation(const wstring& _strKey, UINT _fStarFrame, UINT _fEndFrame
		, float _fSpeed = 1.f, float _fBlendTime = -1.f);

	void DeleteAnimation(const wstring& _strKey);
	void PlayAnimation(const wstring& _strKey, bool _bOnce = false);

	const map<wstring, tAnimationInfo>* GetAnimationInfoMap() { return &m_mapAnim; }
	const wstring& GetCurAnimationKey() { return m_strCurKey; }
	vector<Matrix>& GetFinalBoneMat() { return m_vecFinalBoneMat; }

	bool IsPlay() { return m_bPlay; }
	bool IsStop() { return m_bStop; }
	void SetPlay(bool _bPlay) { m_bPlay = _bPlay; }
	void SetStop(bool _bStop) { m_bStop = _bStop; }
	bool IsAnimFinish() { return m_bIsAnimFinish; }

	float GetCurAnimPrecent() { return m_tCurAnim.fUpdateTime / (m_tCurAnim.fEndTime - m_tCurAnim.fStartTime); }
	int GetCurFrameIdx() { return m_iCurFrame; }
	int GetBlurFrame() { return m_iBlurFrame; }
	const Matrix& GetFinalBoneMatrix(UINT _iBoneIdx) { return m_vecFinalBoneMat[_iBoneIdx]; }
	vector<Matrix>* GetFinalBoneMatPtr() { return &m_vecFinalBoneMat; }
	const Matrix& GetPrevSpineMat() { return m_matPrevSpineMat; }
	CTexture* GetBoneTex() { return m_pBoneTex; }
	CTexture* GetPrevBoneTex() { return m_pPrevBoneTex; }

	void SetRSSpeed(float _fSpeed, float _fCoolTime);

	void SetCurAnimSpeed(float _fSpeed) { m_tCurAnim.fSpeed = _fSpeed; }
	
private:
	void AdjustOffset();
	void FrameProgress();
	void MovePosToFinish();

public:
	CLONE(CAnimator_3D)
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CAnimator_3D();
	virtual ~CAnimator_3D();
};

