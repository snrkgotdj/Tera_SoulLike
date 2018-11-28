#pragma once

#include "global.h"

class CTexture;

struct tInstancingData
{
	Matrix	matWorld;
	Matrix	matWV;
	Matrix	matWVP;
	int		iRowIdx;
	int		iBoneCount;
};

class CInstancingBuffer
{
	SINGLE(CInstancingBuffer);
	
private:
	ID3D11Buffer*			m_pInstancingBuffer;
	UINT					m_iMaxCount;
	vector<tInstancingData> m_vecData;
	vector<Matrix>			m_vecBoneMat;
	int						m_iAnimInstCount;
	int						m_iBoneCount;

public:
	void init();
	void Clear() { m_vecData.clear(); m_vecBoneMat.clear(); m_iAnimInstCount = 0; }
	void AddInstancingData(tInstancingData& _tData) { m_vecData.push_back(_tData); }
	void SetData(CTexture* _pBoneTex);
	UINT GetInstanceCount() { return m_vecData.size(); }
	ID3D11Buffer* GetBuffer() { return m_pInstancingBuffer; }
	void AddInstancingBoneMat(vector<Matrix>& _vecBoneMat);
	int GetBoneCount() { return m_iBoneCount; }

private:
	void Resize(UINT _iCount);
};

// 1. Animator
// 2. Shader Instancing 사용에 따른 분기

