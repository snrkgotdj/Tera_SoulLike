#include "Animator_2D.h"

#include "Animation_2D.h"
#include "Device.h"
#include "PathMgr.h"

#include "shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")


CAnimator_2D::CAnimator_2D()
	: m_pCurAnim(NULL)
	, m_bRepeat(true)
	, m_bStop(false)
{
}

CAnimator_2D::CAnimator_2D(const CAnimator_2D & _other)
	: CComponent(_other)
	, m_pCurAnim(NULL)
	, m_bRepeat(_other.m_bRepeat)
{
	map<wstring, CAnimation*>::const_iterator iter = _other.m_mapAnim.begin();
	for (; iter != _other.m_mapAnim.end(); ++iter)
	{
		CAnimation* pAnim = iter->second->Clone();
		pAnim->SetAnimator(this);
		m_mapAnim.insert(make_pair(iter->first, pAnim));
	}

	if (nullptr != _other.GetCurAnim())
	{
		m_pCurAnim = m_mapAnim.find(_other.GetCurAnim()->GetKey())->second;
	}
}

CAnimator_2D::~CAnimator_2D()
{
	Safe_Delete_Map(m_mapAnim);
}

void CAnimator_2D::PlayAnimation(const wstring & _strKey, bool _bRepeat, UINT _iStartFrameIdx)
{
	m_bRepeat = _bRepeat;

	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter != m_mapAnim.end())
	{
		m_pCurAnim = iter->second;
		m_pCurAnim->SetActive(true);
		m_pCurAnim->SetFrameIdx(_iStartFrameIdx);
	}
}

void CAnimator_2D::PlayAnimationReverse(const wstring & _strKey, bool _bRepeat)
{
	m_bRepeat = _bRepeat;

	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter != m_mapAnim.end())
	{
		m_pCurAnim = iter->second;
		m_pCurAnim->SetActive(true);
		UINT iFrame = ((CAnimation_2D*)m_pCurAnim)->GetFrameSize() - 1;
		m_pCurAnim->SetFrameIdx(iFrame);
		m_pCurAnim->SetReverse(true);
	}
}

void CAnimator_2D::UpdateData()
{
	if (NULL != m_pCurAnim)
	{
		m_pCurAnim->UpdateData();
	}
}

int CAnimator_2D::LateUpdate()
{
	if (NULL != m_pCurAnim && false == m_bStop)
	{
		if (false == m_pCurAnim->IsActive() && m_bRepeat)
		{
			if (m_pCurAnim->GetReverse())
			{
				UINT iFrame = ((CAnimation_2D*)m_pCurAnim)->GetFrameSize() - 1;
				m_pCurAnim->SetFrameIdx(iFrame);
			}
			else
			{
				m_pCurAnim->SetFrameIdx(0);
			}

			m_pCurAnim->SetActive(true);
		}
		m_pCurAnim->LateUpdate();
	}

	return 0;
}

void CAnimator_2D::LoadAnimation_2D(const tDescInfo * _pInfo, const wstring& _strFolderPath)
{
	CAnimation_2D* pAnim = CAnimation_2D::FromDescToAnimation(*_pInfo, _strFolderPath);

	pAnim->SetKey(_pInfo->strAnimName);
	pAnim->SetAnimator(this);
	m_mapAnim.insert(make_pair(_pInfo->strAnimName, pAnim));
}

void CAnimator_2D::Disable()
{
	tAnimInfo tInfo = {};
	tInfo.vAnim.x = 0;

	static const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"AnimationInfo");
	ID3D11Buffer* pCBuffer = NULL;

	// 상수버퍼로 데이터 옮김
	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pCBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pCBuffer);
}

void CAnimator_2D::GetAnimations(vector<tAnimPair>& _vecOut)
{
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.begin();;

	for (; iter != m_mapAnim.end(); ++iter)
	{
		_vecOut.push_back(tAnimPair{iter->first, iter->second});
	}
}

bool CAnimator_2D::IsAnimFinish()
{
	bool bTrue = true;
	if (m_pCurAnim->IsActive())
	{
		bTrue = false;
	}

	return bTrue;
}

void CAnimator_2D::SetTerm(const wstring & _strKey, float _fTerm)
{
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strKey);
	if (iter != m_mapAnim.end())
	{
		((CAnimation_2D*)(*iter).second)->SetTerm(_fTerm);
	}
}

void CAnimator_2D::SetCurTerm(float _fTerm)
{
	((CAnimation_2D*)m_pCurAnim)->SetTerm(_fTerm);
}

float CAnimator_2D::GetCurAnimPrecent()
{	
	float fCur = (float)m_pCurAnim->GetCurFrame();
	float fSize = (float)((CAnimation_2D*)m_pCurAnim)->GetFrameSize() - 1;
	return fCur / fSize;
}

CAnimation * CAnimator_2D::FindAnimation(const wstring & _strKey)
{
	map<wstring, CAnimation*>::iterator iter = m_mapAnim.find(_strKey);

	if (iter == m_mapAnim.end())
		return NULL;

	return iter->second;
}

void CAnimator_2D::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::ANIMATOR_2D;
	fwrite(&iType, sizeof(iType), 1, _pFile);

	UINT iCount = (UINT)m_mapAnim.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	map<wstring, CAnimation*>::iterator iter = m_mapAnim.begin();
	for (; iter != m_mapAnim.end(); ++iter)
	{
		bool b2D = iter->second->Is2DAnim();
		fwrite(&b2D, 1, 1, _pFile);
		iter->second->Save(_pFile);
	}
	
	fwrite(&m_bRepeat, 1, 1, _pFile);
	SaveResourceKey(m_pCurAnim, _pFile);
}

void CAnimator_2D::Load(FILE * _pFile)
{
	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	CAnimation* pAnim = NULL;
	for (UINT i = 0; i < iCount; ++i)
	{
		bool b2D = false;
		fread(&b2D, 1, 1, _pFile);
		
		if (b2D)
		{
			pAnim = new CAnimation_2D;
		}
		else
		{
			//pAnim = new CAnimation_3D;			
		}		
		pAnim->Load(_pFile);
		pAnim->SetAnimator(this);
		m_mapAnim.insert(make_pair(pAnim->GetKey(), pAnim));
	}

	fread(&m_bRepeat, 1, 1, _pFile);
	wstring strKey = LoadResourceKey(_pFile);
	PlayAnimation(strKey, m_bRepeat);
}
