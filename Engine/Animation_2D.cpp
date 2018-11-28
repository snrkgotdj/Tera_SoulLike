#include "Animation_2D.h"

#include "TimeMgr.h"
#include "PathMgr.h"
#include "ResMgr.h"

#include "Device.h"

CAnimation_2D::CAnimation_2D()
{
	m_b2DAnim = true;
}

CAnimation_2D::~CAnimation_2D()
{
}

int CAnimation_2D::LateUpdate()
{
	if (false == m_bActive)
	{		
		return RET_FAILED;
	}

	if (m_bReverse)
	{
		if (m_vecFrame[m_iCurFrame].fTerm <= m_fAccTime)
		{
			m_fAccTime = 0.f;
			m_iCurFrame -= 1;

			if (0 >= m_iCurFrame)
			{
				m_iCurFrame = 0;
				m_bActive = false;
			}
		}
	}
	else
	{
		if (m_vecFrame[m_iCurFrame].fTerm <= m_fAccTime)
		{
			m_fAccTime = 0.f;
			m_iCurFrame += 1;

			if (m_vecFrame.size() <= m_iCurFrame)
			{
				m_iCurFrame = (UINT)m_vecFrame.size() - 1;
				m_bActive = false;
			}
		}
	}

	float fDT = CTimeMgr::GetInst()->DeltaTime();
	m_fAccTime += fDT;

	return 0;
}

void CAnimation_2D::UpdateData()
{
	tAnimInfo tInfo = {};
	tInfo.vAnim.x = 1;
	tInfo.vUV.x = m_vecFrame[m_iCurFrame].vLeftTop.x;
	tInfo.vUV.y = m_vecFrame[m_iCurFrame].vLeftTop.y;
	tInfo.vUV.z = m_vecFrame[m_iCurFrame].fWidth;
	tInfo.vUV.w = m_vecFrame[m_iCurFrame].fHeight;

	const CBUFFER* pBuffer = CDevice::GetInst()->FindConstBuffer(L"AnimationInfo");
		
	// 상수버퍼로 데이터 옮김
	D3D11_MAPPED_SUBRESOURCE tSub = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tSub);
	memcpy(tSub.pData, &tInfo, pBuffer->iSize);
	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->HSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->DSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->CSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->GSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);
	CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (NULL != m_vecFrame[m_iCurFrame].pTex)
		m_vecFrame[m_iCurFrame].pTex->UpdateData(16, (UINT)SHADER_TYPE::ST_PIXEL);	
}

CAnimation_2D* CAnimation_2D::FromDescToAnimation(const tDescInfo & _tDesc
	, const wstring& _strFolderPath)
{
	CAnimation_2D* pAnim = new CAnimation_2D;

	wstring strFolderPath;
	if (-1 == _strFolderPath.find(CPathMgr::GetResPath()))
		strFolderPath = _strFolderPath;
	else
		strFolderPath = _strFolderPath.substr(wcslen(CPathMgr::GetResPath()), _strFolderPath.length());	
	
	if (_tDesc.iContinue)
	{				
		pAnim->SetKey(_tDesc.strAnimName);
		
		WIN32_FIND_DATA tData = {};									
		HANDLE hHandle = FindFirstFile(wstring(CPathMgr::GetResPath() + strFolderPath + L"\\" + L"*.*").c_str(), &tData);

		if (INVALID_HANDLE_VALUE == hHandle)
			assert(NULL);

		while (true)
		{
			if (!wcscmp(_tDesc.strTexName.c_str(), tData.cFileName))
			{
				break;
			}

			if (!FindNextFile(hHandle, &tData))
				assert(NULL);
		}
			
		wstring strTexPath = strFolderPath + L"\\" + tData.cFileName;
		for (UINT j = 0; j < _tDesc.iFrameCountX; ++j)
		{
			tFrameInfo tFrame = {};
			tFrame.vLeftTop = Vec2(0.f, 0.f);
			tFrame.fWidth = 1.f;
			tFrame.fHeight = 1.f;
			tFrame.fTerm = 0.05f;
			tFrame.pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(tData.cFileName, strTexPath.c_str());
			FindNextFile(hHandle, &tData);
			strTexPath = strFolderPath + L"\\" + tData.cFileName;

			pAnim->AddFrame(tFrame);
		}
	}
	else
	{		
		CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(_tDesc.strTexName, strFolderPath + L"//"+ _tDesc.strTexName);
		pAnim->SetKey(_tDesc.strAnimName);

		for (UINT i = 0; i < _tDesc.iFrameCountY; ++i)
		{
			for (UINT j = 0; j < _tDesc.iFrameCountX; ++j)
			{
				tFrameInfo tFrame = {};
				tFrame.pTex = pTex;

				tFrame.vLeftTop.x = _tDesc.vLeftTop.x / pTex->GetWidth() + (j * _tDesc.vSize.x / pTex->GetWidth());
				tFrame.vLeftTop.y = _tDesc.vLeftTop.y / pTex->GetHeight() + (i * _tDesc.vSize.y / pTex->GetHeight());
				tFrame.fWidth = _tDesc.vSize.x / pTex->GetWidth();
				tFrame.fHeight = _tDesc.vSize.y / pTex->GetHeight();

				tFrame.fTerm = 0.05f;

				pAnim->AddFrame(tFrame);
			}
		}
	}
	return pAnim;
}

void CAnimation_2D::SetTerm(float _fTerm)
{
	for (UINT i = 0; i < m_vecFrame.size(); ++i)
	{
		m_vecFrame[i].fTerm = _fTerm;
	}	
}

void CAnimation_2D::Save(FILE * _pFile)
{
	CAnimation::Save(_pFile);

	UINT iCount = (UINT)m_vecFrame.size();
	fwrite(&iCount, sizeof(UINT), 1, _pFile);

	for (UINT i = 0; i < m_vecFrame.size(); ++i)
	{
		fwrite(&m_vecFrame[i].vLeftTop, sizeof(Vec2), 1, _pFile);
		fwrite(&m_vecFrame[i].fWidth, sizeof(float), 1, _pFile);
		fwrite(&m_vecFrame[i].fHeight, sizeof(float), 1, _pFile);
		fwrite(&m_vecFrame[i].fTerm, sizeof(float), 1, _pFile);

		SaveResourceKey(m_vecFrame[i].pTex.GetTarget(), _pFile);
	}
}

void CAnimation_2D::Load(FILE * _pFile)
{
	CAnimation::Load(_pFile);

	UINT iCount = 0;
	fread(&iCount, sizeof(UINT), 1, _pFile);

	tFrameInfo info = {};

	for (UINT i = 0; i < iCount; ++i)
	{
		fread(&info.vLeftTop, sizeof(Vec2), 1, _pFile);
		fread(&info.fWidth, sizeof(float), 1, _pFile);
		fread(&info.fHeight, sizeof(float), 1, _pFile);
		fread(&info.fTerm, sizeof(float), 1, _pFile);

		wstring strKey = LoadResourceKey(_pFile);
		if(!strKey.empty())
			info.pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);

		AddFrame(info);
	}
}
