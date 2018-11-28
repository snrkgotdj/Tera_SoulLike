#include "Animator_3D.h"

#include "Device.h"

#include "ResMgr.h"
#include "Texture.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "GameObject.h"

#include "MeshRender.h"
#include "Transform.h"
#include "Mesh.h"

#include "SceneMgr.h"

CAnimator_3D::CAnimator_3D()
	: m_iCurClip(0)
	, m_iFrameCount(30)
	, m_tCurAnim{}
	, m_bPlay(false)
	, m_bPlayOnce(false)
	, m_iFootIdx(81)
	, m_bChangeAnim(false)
	, m_bIsAnimFinish(false)
	, m_bIsBlening(false)
	, m_fBlendTime(0.f)
	, m_fBlendCoolTime(0.f)
	, m_bBlur(false)
	, m_bStop(false)
	, m_iCurFrame(0)
	, m_iBlurFrame(0)
	, m_pVecFinal(nullptr)
	, m_fRSSpeed(1.f)
	, m_fRSTime(0.f)
	, m_fRSCoolTime(0.f)
	, m_bRS(false)
{
	m_vecPrevBoneMat.resize(10);
}

CAnimator_3D::~CAnimator_3D()
{

}

int CAnimator_3D::Update()
{
	if(m_fRSSpeed > 1.f || m_bRS)
	{
		m_fRSTime += CTimeMgr::GetInst()->DeltaTime();

		if (false == m_bRS && m_fRSTime > 0.05f)
		{
			m_fRSTime = 0.f;
			m_fRSSpeed = 1.f;
		}
	}

	if (m_fRSTime >= m_fRSCoolTime && m_fRSCoolTime > 0.f)
	{
		m_bRS = false;
		m_fRSSpeed = 2.f;
		m_fRSTime = 0.f;
		m_fRSCoolTime = 0.f;
	}

	return 0;
}

int CAnimator_3D::LateUpdate()
{
	if ( (true == m_bStop || false == m_bPlay))
		return RET_FAILED;

	if (false == GetGameObject()->IsActiveAnim() || m_pVecFinal != nullptr)
		return RET_FAILED;

	if (m_bIsBlening)
	{
		m_fBlendTime += CTimeMgr::GetInst()->DeltaTime();
	}

	if (m_fBlendTime >= m_fBlendCoolTime && m_fBlendCoolTime > 0.f)
	{
		m_bIsBlening = false;
		m_fBlendTime = 0.f;
		m_fBlendCoolTime = 0.f;
	}

	FrameProgress();

	for (auto& tParts : m_vecPartsObj)
	{
		tParts.pObj->Transform()->SetBoneMatrix(m_vecFinalBoneMat[tParts.iBoneIdx]);	
	}

	return 0;
}

int CAnimator_3D::DelayUpdate()
{
	if (false == m_bPlay)
		return RET_FAILED;

	FrameProgress();

	for (auto& tParts : m_vecPartsObj)
	{
		tParts.pObj->Transform()->SetBoneMatrix(m_vecFinalBoneMat[tParts.iBoneIdx]);
	}

	return 0;
}

void CAnimator_3D::SetRSSpeed(float _fSpeed, float _fCoolTime)
{
	m_bRS = true;
	m_fRSSpeed = _fSpeed;
	m_fRSCoolTime = _fCoolTime;
}

void CAnimator_3D::AdjustOffset()
{
	for (auto& tParts : m_vecPartsObj)
	{
		tParts.pObj->Transform()->SetBoneMatrix(m_vecFinalBoneMat[tParts.iBoneIdx]);
	}
}

void CAnimator_3D::FrameProgress()
{
	if (false == CSceneMgr::GetInst()->IsPlayMode())
		return;

	float fCurTime = 0.f;

	fCurTime = m_tCurAnim.fStartTime + m_tCurAnim.fUpdateTime;

	// 시간을 통하여 인덱스를 구한다.
	int	iFrameIndex = (int)(fCurTime * m_iFrameCount);
	m_iCurFrame = iFrameIndex;
	// 본 개수만큼 반복하며 현재 시간에 맞게 모든 본 행렬을 모두 보간해준다.
	for (size_t i = 0; i < m_pVecBones->size(); ++i)
	{
		if (m_pVecBones->at(i).vecKeyFrame.empty())
		{
			// 키프레임 별 행렬이 없는 본일 경우
			m_vecFinalBoneMat[i] = XMMatrixIdentity();
			continue;
		}
	
		int	iFrameNextIndex = 0;

		if (m_bIsBlening)
		{
			iFrameIndex = m_tPrevAnim.iEndFrame;
			if (iFrameIndex >= (int)m_pVecBones->at(i).vecKeyFrame.size())
				iFrameIndex = m_pVecBones->at(i).vecKeyFrame.size() - 1;

			iFrameNextIndex = m_tCurAnim.iStartFrame;
		}
		else
		{
			if (iFrameIndex >= (int)m_tCurAnim.iEndFrame - 1 && (int)m_tCurAnim.iEndFrame != 0)
			{
				iFrameIndex = (int)m_tCurAnim.iEndFrame - 1;
				iFrameNextIndex = iFrameIndex;
			}
			else
			{
				iFrameNextIndex = iFrameIndex + 1;

				if (iFrameNextIndex >= (int)m_tCurAnim.iEndFrame - 1)
				{
					iFrameNextIndex -= 1;
				}
			}
		}

		const tMTKeyFrame& tKeyFrame = m_pVecBones->at(i).vecKeyFrame[iFrameIndex];
		const tMTKeyFrame& tNextKeyFrame = m_pVecBones->at(i).vecKeyFrame[iFrameNextIndex];

		float fFrameTime = tKeyFrame.fTime;
		float fNextFrameTime = tNextKeyFrame.fTime;

		// 프레임간의 시간에 따른 비율을 구해준다.
		//float fPercent = (fNextFrameTime - fFrameTime) / (1.f / m_iFrameCount);

		float fPercent = 0.f;

		if (m_bIsBlening)
		{
			fPercent = m_fBlendTime / m_fBlendCoolTime;
		}
		else
		{
			fPercent = (float)(iFrameIndex - m_tCurAnim.iStartFrame) / (float)(m_tCurAnim.iEndFrame - m_tCurAnim.iStartFrame);
		}

		XMVECTOR vS1 = tKeyFrame.vScale.Convert();
		XMVECTOR vS2 = tNextKeyFrame.vScale.Convert();

		XMVECTOR vT1 = tKeyFrame.vTranslate.Convert();
		XMVECTOR vT2 = tNextKeyFrame.vTranslate.Convert();

		XMVECTOR vQ1 = XMLoadFloat4(&tKeyFrame.qRot);
		XMVECTOR vQ2 = XMLoadFloat4(&tNextKeyFrame.qRot);

		XMVECTOR vS = XMVectorLerp(vS1, vS2, fPercent);
		XMVECTOR vT = XMVectorLerp(vT1, vT2, fPercent);
		XMVECTOR vQ = XMQuaternionSlerp(vQ1, vQ2, fPercent);

		XMVECTOR vQZero = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		// 오프셋 행렬을 곱하여 최종 본행렬을 만들어낸다.				
		m_vecFinalBoneMat[i] = m_pVecBones->at(i).matOffset * XMMatrixAffineTransformation(vS, vQZero, vQ, vT);
	}

	if (m_tCurAnim.fUpdateTime < 0.0001f)
		m_matPrevSpineMat = m_vecFinalBoneMat[4];

	// 현재 재생중인 Clip 의 시간을 진행한다.	
	if (m_tCurAnim.fUpdateTime >= m_tCurAnim.fEndTime - m_tCurAnim.fStartTime)
	{
		if (m_bPlayOnce)
		{
			m_bIsAnimFinish = true;
			m_tCurAnim = {};
			m_strCurKey = L"";
			m_bPlay = false;
			return;
		}
		else
		{
			m_tCurAnim.fUpdateTime = 0.f;
			m_bIsAnimFinish = true;
		}


	}
	else
	{
		if (false == m_bIsBlening)
			m_tCurAnim.fUpdateTime += CTimeMgr::GetInst()->DeltaTime() * m_tCurAnim.fSpeed * m_fRSSpeed;

		m_bIsAnimFinish = false;
	}
}

void CAnimator_3D::MovePosToFinish()
{
	Vec3 StartPos = Vec3(m_matPrevSpineMat._41, m_matPrevSpineMat._42, m_matPrevSpineMat._43);
	Vec3 FinishPos = Vec3(m_vecFinalBoneMat[4]._41, m_vecFinalBoneMat[4]._42, m_vecFinalBoneMat[4]._43);

	Vec3 vPos = Transform()->GetLocalPos();
	Vec3 vScale = Transform()->GetLocalScale();

	FinishPos *= vScale;
	StartPos *= vScale;

	Vec3 vDist = FinishPos - StartPos;

	vPos += Transform()->GetDir(DIR_TYPE::RIGHT) * vDist.Distance();

	Transform()->SetLocalPos(vPos);
}

void CAnimator_3D::SetPartsObj(UINT _iBoneIdx, CGameObject * _pObj)
{
	tPARTSOBJ tObj = {};
	tObj.iBoneIdx = _iBoneIdx;
	tObj.pObj = _pObj;
	_pObj->SetPartsIdx(_iBoneIdx);
	m_vecPartsObj.push_back(tObj);

	GetGameObject()->AddChildObj(_pObj);
	_pObj->Transform()->SetLocalPos(Vec3());
}

void CAnimator_3D::SetPartsObjAtSave(UINT _iBoneIdx, CGameObject * _pObj)
{
	tPARTSOBJ tObj = {};
	tObj.iBoneIdx = _iBoneIdx;
	tObj.pObj = _pObj;
	_pObj->SetPartsIdx(_iBoneIdx);
	m_vecPartsObj.push_back(tObj);
}

void CAnimator_3D::SetBlur(bool _bBlur, int _bPrevFrame)
{
	m_bBlur = _bBlur;
	m_iBlurFrame = _bPrevFrame;
	if (m_bBlur == false)
		m_iBlurFrame = 0;

	else
	{
		if (nullptr == m_pPrevBoneTex)
		{
			m_pPrevBoneTex = CResMgr::GetInst()->CreateTexture(L"Blur"
				, MeshRender()->GetMesh()->GetBones()->size() * 4, 1
				, D3D11_BIND_SHADER_RESOURCE
				, DXGI_FORMAT_R32G32B32A32_FLOAT
				, D3D11_USAGE_DYNAMIC);
		}
	}
}

void CAnimator_3D::UpdateData()
{
	if (NULL == m_pBoneTex)
		return;
	tAnimInfo tInfo = {};
	if (GetGameObject()->IsActiveAnim())
	{
		// Bone Texture Update
		m_pBoneTex->SetData(&m_vecFinalBoneMat[0], sizeof(Matrix) * m_vecFinalBoneMat.size());
		m_pBoneTex->UpdateData(17, (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);
		// Animation 
		
		if (true == m_bPlay && true == m_bBlur && 0 != m_vecPrevBoneMat[0].size())
		{
			m_pPrevBoneTex->SetData(&m_vecPrevBoneMat[0][0], sizeof(Matrix) * m_vecPrevBoneMat[0].size());
			m_pPrevBoneTex->UpdateData(18, (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);
			tInfo.vAnim.y = 1.f;
		}
		else
			tInfo.vAnim.y = 0.f;
	}

	else
	{
		if (m_pVecFinal != nullptr)
		{
			// Bone Texture Update
			m_pBoneTex->SetData(&m_pVecFinal->at(0), sizeof(Matrix) * m_pVecFinal->size());
			m_pBoneTex->UpdateData(17, (UINT)SHADER_TYPE::ST_VERTEX | (UINT)SHADER_TYPE::ST_PIXEL);
			// Animation 
			tInfo.vAnim.y = 0.f;
		}
	}
	
	
	
	if (m_bPlay || m_bStop)
		tInfo.vAnim.x = 1.f;
	else
		tInfo.vAnim.x = 0.f;

	static const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"AnimationInfo");

	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &tInfo, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	//이전 본행렬정보
	if (true == m_bPlay && true == m_bBlur)
	{
		for (int i = 0; i < m_iBlurFrame; ++i)
		{
			m_vecPrevBoneMat[i] = m_vecPrevBoneMat[i + 1];
		}

		m_vecPrevBoneMat[m_iBlurFrame] = m_vecFinalBoneMat;
	}
	
}

void CAnimator_3D::ClearData()
{
	// Bone Texture Update	
	CTexture::ClearData((UINT)SHADER_TYPE::ST_VERTEX, 17);

	// Animation 
	static const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"AnimationInfo");
	ID3D11Buffer* pCBuffer = NULL;

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pCBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pCBuffer);
}

int CAnimator_3D::AddAnimation(const wstring & _strKey, UINT _fStarFrame, UINT _fEndFrame
	, float _fSpeed, float _fBlendTime)
{
	map<wstring, tAnimationInfo>::iterator iter = m_mapAnim.find(_strKey);
	if (m_mapAnim.end() != iter)
		return RET_FAILED;

	tAnimationInfo tAnim = {};
	tAnim.iStartFrame = _fStarFrame;
	tAnim.iEndFrame = _fEndFrame;
	tAnim.fStartTime = (float)tAnim.iStartFrame / m_iFrameCount;
	tAnim.fEndTime = (float)tAnim.iEndFrame / m_iFrameCount;
	tAnim.fSpeed = _fSpeed;
	tAnim.fBenldTime = _fBlendTime;

	m_mapAnim.insert(make_pair(_strKey, tAnim));

	return RET_SUCCESS;
}

void CAnimator_3D::DeleteAnimation(const wstring & _strKey)
{
	map<wstring, tAnimationInfo>::iterator iter = m_mapAnim.find(_strKey);
	if (m_mapAnim.end() == iter)
		return;

	if (iter->first == m_strCurKey)
	{
		m_bPlay = false;
		m_tCurAnim = {};
	}

	m_mapAnim.erase(iter);
}

void CAnimator_3D::PlayAnimation(const wstring & _strKey, bool _bOnce)
{
	if (_strKey == m_strCurKey)
		return;

	map<wstring, tAnimationInfo>::iterator iter = m_mapAnim.find(_strKey);
	if (m_mapAnim.end() == iter)
	{
		m_tCurAnim = {};
		m_strCurKey = L"";
		m_bPlay = false;
		MessageBox(nullptr, L"재생시킨 애니메이션이 없습니다. 이름을 다시확인하세요", L"애니메이션 재생 오류", MB_OK);
		assert(nullptr);
		return;
	}

	m_tPrevAnim = m_tCurAnim;
	m_tCurAnim = iter->second;
	m_tCurAnim.fUpdateTime = 0.f;
	m_strCurKey = _strKey;
	m_bPlay = true;
	m_bPlayOnce = _bOnce;

	if (m_tPrevAnim.fBenldTime > 0.f)
	{
		m_bIsBlening = true;
		m_fBlendCoolTime = m_tPrevAnim.fBenldTime;
	}

	// 애니메이션 변경시 위치가 바뀌어있으면 그위치로 이동
	MovePosToFinish();

	m_fRSSpeed = 1.f;
}

void CAnimator_3D::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::ANIMATOR_3D;
	fwrite(&iType, sizeof(iType), 1, _pFile);

	fwrite(&m_bBlur, sizeof(bool), 1, _pFile);

}

void CAnimator_3D::Load(FILE * _pFile)
{
	fread(&m_bBlur, sizeof(bool), 1, _pFile);
	SetBlur(m_bBlur);


	CMesh* pMesh = MeshRender()->GetMesh();

	SetBones(pMesh->GetBones());
	//SetAnimClip(pMesh->GetAnimClip());
	SetBoneTex(pMesh->GetBoneTex());
}
