#include "Terrain.h"

#include "Device.h"
#include "SceneMgr.h"
#include "KeyMgr.h"
#include "ResMgr.h"

#include "GameObject.h"
#include "MeshRender.h"
#include "Camera.h"
#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "NaviMesh.h"
#include "Texture.h"
#include "TimeMgr.h"

#include <DirectXCollision.h>
using namespace DirectX::TriangleTests;

struct tRGBA {
	BYTE r, g, b, a;
};

CTerrain::CTerrain()
	: m_eMod(TERRAIN_MOD::END)
	, m_pHeightMapTex(NULL)
	, m_pWeightTex(NULL)
	, m_iCurBrushIdx(0)
	, m_iXFace(0)
	, m_iZFace(0)
	, m_fBrushScale(1.f)
	, m_fBrushRadius(3200.f)
	, m_iPrevIdx(0)
	, m_bAway(false)
	, m_bUseTess(false)
	, m_iTileIdx(0)
	, m_iCurPickPatch(-1)
{

}

CTerrain::~CTerrain()
{
}

void CTerrain::Awake()
{
}

void CTerrain::Start()
{
}

int CTerrain::Update()
{
	return 0;
}

int CTerrain::DelayUpdate()
{
	return 0;

	// KeyCheck();

	/*if (true == KEY_INPUT(KEY_F1, STATE_TAB))
	{
		m_eMod = TERRAIN_MOD::HEIGHTMAP_MOD;
	}
	else if (true == KEY_INPUT(KEY_F2, STATE_TAB))
	{
		m_eMod = TERRAIN_MOD::HEIGHT_RESET_MOD;
	}
	else if (true == KEY_INPUT(KEY_F3, STATE_TAB))
	{
		m_eMod = TERRAIN_MOD::HEIGHTMAP_REVERSE_MOD;
	}
	else if (true == KEY_INPUT(KEY_F4, STATE_TAB))
	{
		m_eMod = TERRAIN_MOD::HEIGHTMAP_SPLAT_MOD;
	}
	else if (true == KEY_INPUT(KEY_F5, STATE_TAB))
	{
		m_eMod = TERRAIN_MOD::END;
	}
	else if (true == KEY_INPUT(KEY_F6, STATE_TAB))
	{
		EditWeightAll();
	}*/

	if (m_eMod == TERRAIN_MOD::END)
		return 0;

	 m_iCurPickPatch = Picking();
	if (-1 == m_iCurPickPatch && m_bAway)
		return 0;

	BrushUpdate(m_iCurPickPatch);

	// 피킹
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LBTN, KEY_STATE::STATE_HOLD))
	{	
		if (-1 != m_iCurPickPatch)
		{
			switch (m_eMod)
			{
			case TERRAIN_MOD::HEIGHTMAP_MOD:
				EditHeight(m_iCurPickPatch);
				break;
			case TERRAIN_MOD::HEIGHTMAP_REVERSE_MOD:
				EditHeightReverse2(m_iCurPickPatch);
				break;
			case TERRAIN_MOD::HEIGHT_RESET_MOD:
				EditHeightReset(m_iCurPickPatch);
				break;
			case TERRAIN_MOD::SPLAT_MOD:
				EditWeight(m_iCurPickPatch);
				break;
			case TERRAIN_MOD::NAVIMESH_MOD:
				EditNaviMesh(m_iCurPickPatch);
				break;
			case TERRAIN_MOD::HEIGHTMAP_SPLAT_MOD:
				EditHeightSplat(m_iCurPickPatch);
				break;
			}	
		}
	}

	/*if (true == KEY_INPUT(KEY_F1, STATE_TAB))
	{
		switch (m_eMod)
		{
		case TERRAIN_MOD::HEIGHTMAP_MOD:
			m_eMod = TERRAIN_MOD::HEIGHTMAP_REVERSE_MOD;
			break;
		case TERRAIN_MOD::NAVIMESH_MOD:
			CheckAllNavi();
			break;
		}	
	}*/

	return 0;
}

void CTerrain::KeyCheck()
{
	if (CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_TAB, STATE_TAB))
	{
		if (m_eMod == TERRAIN_MOD::END)
		{
			m_eMod = TERRAIN_MOD::HEIGHTMAP_MOD;

		}

		else if (m_eMod == TERRAIN_MOD::HEIGHTMAP_MOD)
		{
			m_eMod = TERRAIN_MOD::SPLAT_MOD;

		}

		else
		{
			m_eMod = TERRAIN_MOD::END;
		}
		
		int a = 0;
	}

	if (true ==KEY_INPUT(KEY_F2, STATE_TAB))
	{
		//ResetHeight();
	}

}

int CTerrain::Picking(Vec3& _vPos)
{
	// Camera 에서의 Ray
	CGameObject* pToolCamObj = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
	tRay ray = pToolCamObj->Camera()->GetRay();

	// Local 로 보내기 위한 월드행렬의 역행렬
	Matrix matWorld = Transform()->GetWorldMat();
	matWorld = XMMatrixInverse(NULL, matWorld);
	ray.vOrigin = XMVector3TransformCoord(ray.vOrigin.Convert(), matWorld);
	ray.vDir = XMVector3TransformNormal(ray.vDir.Convert(), matWorld);
	ray.vDir.Normalize();

	for (int i = 0; i < 3; ++i)
	{
		if (isnan(ray.vOrigin[i])) 
			return -1;
	}

	// 충돌 체크를 하기위한 버텍스, 인덱스 버퍼를 가져온다.
	CMesh* pMesh = MeshRender()->GetMesh();
	VTX* pVtx = pMesh->GetVtxSysMem();
	WORD* pIdx = (WORD*)pMesh->GetIdxSysMem();

	UINT iPatchCount = pMesh->GetIdxCount() / 3;
	float fDist = 0.f;
	UINT i = 0;

	for (; i < iPatchCount; ++i)
	{
		if (Intersects(ray.vOrigin.Convert(), ray.vDir.Convert()
			, pVtx[pIdx[i * 3]].vPos.Convert()
			, pVtx[pIdx[i * 3 + 1]].vPos.Convert()
			, pVtx[pIdx[i * 3 + 2]].vPos.Convert()
			, fDist))
		{
			_vPos = pVtx[pIdx[i * 3]].vPos + pVtx[pIdx[i * 3 + 1]].vPos;
			_vPos += pVtx[pIdx[i * 3 + 2]].vPos;
			_vPos /= 3.f;
			return i;
		}
	}

	return -1;
}

void CTerrain::SetBlood(Vec3 & _vPos)
{
	UINT iIdx = GetPatchIdxByPos(_vPos);

	m_iCurBrushIdx = 2;
	EditWeightBlood(iIdx);
}

void CTerrain::GetBrushRatio(UINT _iPatchIdx, Vec2 & _vBrushScale, Vec2 & _vBrushPos)
{
	// 브러쉬 범위가 전체 지형내에서 차지하는 비율값 구하기
	Vec3 vScale = Transform()->GetWorldScale();

	_vBrushScale = Vec2((m_fBrushRadius * m_fBrushScale) / (m_iXFace * vScale.x * 2)
		, (m_fBrushRadius * m_fBrushScale) / (m_iZFace * vScale.z * 2));

	// 피킹된 위치가 상대적으로 차지하는 위치 비율값(좌 하단 기준)
	UINT iZFacePos = _iPatchIdx / (m_iXFace * 2);
	UINT iXFacePos = _iPatchIdx % (m_iXFace * 2);

	_vBrushPos = Vec2((float)iXFacePos / ((float)m_iXFace * 2.f),
					  1.f - (float)iZFacePos / (float)m_iZFace);
}

void CTerrain::BrushUpdate(UINT _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	Vec4 vBrushRatio(vBrushPos.x, vBrushPos.y, vBrushScale.x, vBrushScale.y);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_VEC4_0, &vBrushRatio);

	int Tess = m_bUseTess;
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_INT_0, &Tess);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_INT_1, &m_eMod);
}

void CTerrain::EditHeight(int _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	float* pPixel = (float*)pHeightMap->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY)* (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;
			fAlpha /= 255.f;

			if (fAlpha >= 0.5f)
				fAlpha = 1.f;
			
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			pPixel[(int)fWidth * i + j] += 0.2f * fAlpha;
		}
	}

	pHeightMap->Reset();
}

void CTerrain::EditHeightReverse(int _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	float* pPixel = (float*)pHeightMap->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	for (int i = 0; i < (int)m_iXFace * 16; ++i)
	{
		for (int j = 0; j < (int)m_iZFace * 16; ++j)
		{
			if (i >= iLTY && i < iLTY + iBrushSizeY * 2 && j >= iLTX && j < iLTX + iBrushSizeY * 2)
			{
				float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY)* (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;
				fAlpha /= 255.f;
				if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
					break;

				if(fAlpha >= 0.3)
					pPixel[(int)fWidth * i + j] = 0;
				else
					pPixel[(int)fWidth * i + j] += 0.2f * (1 - fAlpha);
			}

			else
			{
				pPixel[(int)fWidth * i + j] += 0.2f;
			}
		}
	}

	pHeightMap->Reset();

}

void CTerrain::EditHeightReset(int _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	float* pPixel = (float*)pHeightMap->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY)* (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;
			fAlpha /= 255.f;
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			if (fAlpha != 0)
				pPixel[(int)fWidth * i + j] = 0.f;
		}
	}

	pHeightMap->Reset();
}

void CTerrain::EditHeightReverse2(int _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	float* pPixel = (float*)pHeightMap->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY)* (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;
			fAlpha /= 255.f;

			if (fAlpha >= 0.5f)
				fAlpha = 1.f;

			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			pPixel[(int)fWidth * i + j] -= 0.2f * fAlpha;
		}
	}

	pHeightMap->Reset();
}

void CTerrain::EditHeightSplat(int _iPatchIdx)
{
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	float* pPixel = (float*)pHeightMap->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	float fAver = 0.f;
	UINT iCount = 0;

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			fAver += pPixel[(int)fWidth * i + j];
			iCount++;
		}
	}

	fAver /= iCount;

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			if (pPixel[(int)fWidth * i + j] < fAver)
			{
				pPixel[(int)fWidth * i + j] += 0.01f;
			}
			else
				pPixel[(int)fWidth * i + j] -= 0.05f;
		}
	}

	pHeightMap->Reset();
}

void CTerrain::ResetHeight()
{
	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float* pPixel = (float*)pHeightMap->GetPixel();

	for (int i = 0; i < (int)m_iXFace * 16; ++i)
	{
		for (int j = 0; j < (int)m_iZFace * 16; ++j)
		{			
			pPixel[(int)fWidth * i + j] = 0.f;
		}
	}
	pHeightMap->Reset();
}

void CTerrain::EditWeight(int _iPatchIdx)
{
	float fDelta = CTimeMgr::GetInst()->DeltaTime();

	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pWeightTex = m_pWeightTex;
	float fWidth = pWeightTex->GetWidth();
	float fHeight = pWeightTex->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	tRGBA* pPixel = (tRGBA*)pWeightTex->GetPixel();

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	float fWeight = 0.5f;

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY) * (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;

			if (fAlpha != 0.f)
				fAlpha = 1.f;

			if (fAlpha == 0.f)
				continue;

			if (0 == m_iTileIdx)
			{
				if (pPixel[(int)fWidth * i + j].r == 255)
					continue;

				float fVale = (float)pPixel[(int)fWidth * i + j].r + (fWeight * fAlpha) * fDelta * 200.f;

				if (fVale >= 255)
					pPixel[(int)fWidth * i + j].r = 255;
				else
					pPixel[(int)fWidth * i + j].r += (BYTE)(fWeight * fAlpha * fDelta * 200.f);
			}
			else if (1 == m_iTileIdx)
			{
				if (pPixel[(int)fWidth * i + j].g == 255)
					continue;

				float fVale = (float)pPixel[(int)fWidth * i + j].g + (fWeight * fAlpha) * fDelta * 200.f;
				if (fVale >= 255)
					pPixel[(int)fWidth * i + j].g = 255;
				else
					pPixel[(int)fWidth * i + j].g += (BYTE)(fWeight * fAlpha * fDelta * 200.f);
			}
			else if (2 == m_iTileIdx)
			{
				if (pPixel[(int)fWidth * i + j].b == 255)
					continue;

				float fVale = (float)pPixel[(int)fWidth * i + j].b + (fWeight * fAlpha)* fDelta * 200.f;
				if (fVale >= 255)
					pPixel[(int)fWidth * i + j].b = 255;
				else
					pPixel[(int)fWidth * i + j].b += (BYTE)(fWeight * fAlpha* fDelta * 200.f);
			}
			else if (3 == m_iTileIdx)
			{
				if (pPixel[(int)fWidth * i + j].a == 255)
					continue;

				float fVale = (float)pPixel[(int)fWidth * i + j].a + (fWeight * fAlpha) * fDelta * 200.f;
				if (fVale >= 255)
					pPixel[(int)fWidth * i + j].a = 255;
				else
					pPixel[(int)fWidth * i + j].a += (BYTE)(fWeight * fAlpha* fDelta * 200.f);
			}

			
			float fValue = fWeight * fAlpha* fDelta * 200.f;
			fValue /= 3.f;

			if (m_iTileIdx != 0)
			{
				if ((float)pPixel[(int)fWidth * i + j].r - fValue <= 0)
					pPixel[(int)fWidth * i + j].r = 0;
				else
					pPixel[(int)fWidth * i + j].r = (BYTE)((float)pPixel[(int)fWidth * i + j].r - fValue);
			}

			if (m_iTileIdx != 1)
			{
				if ((float)pPixel[(int)fWidth * i + j].g - fValue <= 0)
					pPixel[(int)fWidth * i + j].g = 0;
				else
					pPixel[(int)fWidth * i + j].g = (BYTE)((float)pPixel[(int)fWidth * i + j].g - fValue);
			}

			if (m_iTileIdx != 2)
			{
				if ((float)pPixel[(int)fWidth * i + j].b - fValue <= 0)
					pPixel[(int)fWidth * i + j].b = 0;
				else
					pPixel[(int)fWidth * i + j].b = (BYTE)((float)pPixel[(int)fWidth * i + j].b - fValue);
			}

			if (m_iTileIdx != 3)
			{
				if ((float)pPixel[(int)fWidth * i + j].a - fValue <= 0)
					pPixel[(int)fWidth * i + j].a = 0;
				else
					pPixel[(int)fWidth * i + j].a = (BYTE)((float)pPixel[(int)fWidth * i + j].a - fValue);
			}
		}
	}

	pWeightTex->Reset();
}

void CTerrain::EditWeightAll()
{
	CTexture* pWeightTex = m_pWeightTex;
	tRGBA* pPixel = (tRGBA*)pWeightTex->GetPixel();

	float fWidth = pWeightTex->GetWidth();
	float fHeight = pWeightTex->GetHeight();

	for (int i = 0; i < fWidth * fHeight; ++i)
	{
		pPixel[i].r = 255;
	}

	pWeightTex->Reset();
}

void CTerrain::EditWeightBlood(int _iPatchIdx)
{
	m_fBrushScale = 1.f;
	m_fBrushRadius = 100.f;

	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CTexture* pWeightTex = m_pWeightTex;
	float fWidth = pWeightTex->GetWidth();
	float fHeight = pWeightTex->GetHeight();

	// 픽셀 기준 브러쉬의 크기(단위는 pixel)
	int iBrushSizeX = (int)(vBrushScale.x * fWidth);
	int iBrushSizeY = (int)(vBrushScale.y * fHeight);

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);

	iCenterY -= 10;

	int iLTX = iCenterX - iBrushSizeX;
	int iLTY = iCenterY - iBrushSizeY;

	if (iLTX < 0)
		iLTX = 0;
	if (iLTY < 0)
		iLTY = 0;

	tRGBA* pPixel = (tRGBA*)pWeightTex->GetPixel();

	tRGBA* pBrushRGB = (tRGBA*)m_vecBrushTex[m_iCurBrushIdx]->GetPixel();

	float fBrushWidth = m_vecBrushTex[m_iCurBrushIdx]->GetWidth();
	float fBrushHeight = m_vecBrushTex[m_iCurBrushIdx]->GetHeight();

	UINT fBrushRatioX = (UINT)(fBrushWidth / (iBrushSizeX * 2));
	UINT fBrushRatioY = (UINT)(fBrushHeight / (iBrushSizeY * 2));

	for (int i = iLTY; i < iLTY + iBrushSizeY * 2; ++i)
	{
		for (int j = iLTX; j < iLTX + iBrushSizeX * 2; ++j)
		{
			if (i >= (int)m_iXFace * 16 || j >= (int)m_iZFace * 16)
				break;

			float fAlpha = (float)pBrushRGB[((i - iLTY) * fBrushRatioY) * (UINT)fBrushWidth + ((j - iLTX) * fBrushRatioX)].a;
			if (fAlpha != 0.f)
				fAlpha = 1.f;

			if (255 < pPixel[(int)fWidth * i + j].a + 255 * (UINT)fAlpha)
				pPixel[(int)fWidth * i + j].a = 255;
			else
				pPixel[(int)fWidth * i + j].a += 255 * (UINT)fAlpha;
		}
	}

	pWeightTex->Reset();
}

void CTerrain::EditNaviMesh(int _iPatchIdx)
{
	
	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(_iPatchIdx, vBrushScale, vBrushPos);

	CMesh* pMesh = MeshRender()->GetMesh();
	VTX* pVtxMem = pMesh->GetVtxSysMem();
	UINT iVtxCount = pMesh->GetVtxCount();
	WORD* pIdxMem = (WORD*)pMesh->GetIdxSysMem();

	bool bResult = m_NaviMesh.AddCell(pVtxMem[pIdxMem[_iPatchIdx * 3]].vPos, pVtxMem[pIdxMem[_iPatchIdx * 3 + 1]].vPos, pVtxMem[pIdxMem[_iPatchIdx * 3 + 2]].vPos, _iPatchIdx);
	
	if (false == bResult)
		return;

	pVtxMem[pIdxMem[_iPatchIdx * 3]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	pVtxMem[pIdxMem[_iPatchIdx * 3 + 1]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	pVtxMem[pIdxMem[_iPatchIdx * 3 + 2]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);

	pMesh->ResetVTX(pVtxMem, iVtxCount);
}

void CTerrain::CheckAllNavi()
{
	CMesh* pMesh = MeshRender()->GetMesh();
	VTX* pVtxMem = pMesh->GetVtxSysMem();
	UINT iVtxCount = pMesh->GetVtxCount();
	WORD* pIdxMem = (WORD*)pMesh->GetIdxSysMem();
	UINT iPatchCount = pMesh->GetIdxCount() / 3;
	

	for (UINT i = 0; i < iPatchCount; ++i)
	{
		bool bResult = m_NaviMesh.AddCell(pVtxMem[pIdxMem[i * 3]].vPos, pVtxMem[pIdxMem[i * 3 + 1]].vPos, pVtxMem[pIdxMem[i * 3 + 2]].vPos, i);

		if (false == bResult)
			continue;

		pVtxMem[pIdxMem[i * 3]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);
		pVtxMem[pIdxMem[i * 3 + 1]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);
		pVtxMem[pIdxMem[i * 3 + 2]].vColor = Vec4(0.f, 0.f, 0.f, 1.f);
	}

	pMesh->ResetVTX(pVtxMem, iVtxCount);
}


void CTerrain::Init()
{
	Vec4 vData;

	// World Distance of Texel
	vData.x = (float)m_iXFace * Transform()->GetWorldScale().x / m_pHeightMapTex->GetWidth();
	vData.y = (float)m_iZFace * Transform()->GetWorldScale().z / m_pHeightMapTex->GetHeight();

	vData.x *= 8;
	vData.y *= 8;

	// World Y Scale
	vData.z = Transform()->GetWorldScale().y;

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_VEC4_1, &vData);
	LoadTileTexture();
}

void CTerrain::SetHeightMap(CTexture * _pHeightTex)
{
	m_pHeightMapTex = _pHeightTex;
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &m_pHeightMapTex);
}

void CTerrain::SetWeightMap(CTexture * _pWeightTex)
{
	m_pWeightTex = _pWeightTex;
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_1, &m_pWeightTex);
}

void CTerrain::SetFaceCount(UINT _iXFace, UINT _iZFace)
{
	m_iXFace = _iXFace;
	m_iZFace = _iZFace;

	float fXFace = (float)m_iXFace;
	float fZFace = (float)m_iZFace;

	CGameObject* pObj = GetGameObject();
	pObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fXFace);
	pObj->MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_1, &fZFace);

	m_NaviMesh.SetFaceCount(m_iXFace, m_iZFace);
}

void CTerrain::SetBrush(int _iIdx)
{
	m_iCurBrushIdx = _iIdx;
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_2, &m_vecBrushTex[m_iCurBrushIdx]);
}

int CTerrain::GetWorldPosbyTex(Vec3& _vPos)
{
	int iPickingIdx = Picking(_vPos);
	if (-1 == iPickingIdx)
		return RET_FAILED;

	Vec2 vBrushScale, vBrushPos;
	GetBrushRatio(iPickingIdx, vBrushScale, vBrushPos);

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	// 피킹위치에 해당하는 픽셀 인덱스
	int iCenterX = (int)(vBrushPos.x * fWidth);
	int iCenterY = (int)(vBrushPos.y * fHeight);
	int iIdx = (iCenterY * (int)fWidth) + iCenterX;

	float* pPixel = (float*)pHeightMap->GetPixel();

	_vPos.x *= Transform()->GetLocalScale().x;
	_vPos.y = pPixel[iIdx] * Transform()->GetLocalScale().y;
	_vPos.z *= Transform()->GetLocalScale().z;

	return RET_SUCCESS;
}

Vec3 CTerrain::GetTerrainPos(const Vec3 & _vPos)
{
	Vec3 vTerrainPos = Transform()->GetLocalPos();
	Vec3 vTerrainScale = Transform()->GetLocalScale();

	float fDistX = _vPos.x - vTerrainPos.x;
	float fDistZ = _vPos.z - vTerrainPos.z;

	float fTerrainX = vTerrainScale.x * m_iXFace;
	float fTerrainZ = vTerrainScale.z * m_iZFace;

	Vec2 vUV = Vec2(fDistX / fTerrainX, 1 - (fDistZ / fTerrainZ));

	CTexture* pHeightMap = *((CTexture**)MeshRender()->GetSharedMaterial()->GetData(SHADER_PARAM::SP_TEX_0));
	float fWidth = pHeightMap->GetWidth();
	float fHeight = pHeightMap->GetHeight();

	int iCenterX = (int)(vUV.x * fWidth);
	int iCenterY = (int)(vUV.y * fHeight);

	float* pPixel = (float*)pHeightMap->GetPixel();

	vTerrainPos = _vPos;

	if (iCenterX < fWidth - 1 && iCenterY < fHeight - 1 && iCenterX > 1 && iCenterY > 1)
	{
		float fHeight = 0.f;
		int iIdx = (iCenterY * (int)fWidth) + iCenterX;
		fHeight += pPixel[iIdx];
		
		iIdx = ((iCenterY + 1) * (int)fWidth) + iCenterX;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY + 1) * (int)fWidth) + iCenterX + 1;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY) * (int)fWidth) + iCenterX + 1;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY - 1) * (int)fWidth) + iCenterX + 1;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY - 1) * (int)fWidth) + iCenterX;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY - 1) * (int)fWidth) + iCenterX - 1;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY) * (int)fWidth) + iCenterX - 1;
		fHeight += pPixel[iIdx];

		iIdx = ((iCenterY + 1) * (int)fWidth) + iCenterX - 1;
		fHeight += pPixel[iIdx];

		vTerrainPos.y = (fHeight / 9) * vTerrainScale.y;
	}

	return vTerrainPos;
}

UINT CTerrain::GetPatchIdxByPos(Vec3 & _vPos)
{
	Vec3 vTerrainPos = Transform()->GetLocalPos();
	Vec3 vTerrainScale = Transform()->GetLocalScale();

	float fDistX = _vPos.x - vTerrainPos.x;
	float fDistZ = _vPos.z - vTerrainPos.z;

	float fTerrainX = vTerrainScale.x * m_iXFace;
	float fTerrainZ = vTerrainScale.z * m_iZFace;

	Vec2 vUV = Vec2(fDistX / fTerrainX, fDistZ / fTerrainZ);

	int iCenterX = (int)(vUV.x * m_iXFace);
	int iCenterZ = (int)(vUV.y * m_iZFace);

	if (iCenterX < 0 && iCenterX >= (int)m_iXFace
		&& iCenterZ < 0 && iCenterZ >= (int)m_iXFace)
	{
		return -1;
	}

	UINT iIdx = iCenterZ * m_iXFace * 2 + iCenterX * 2;

	float fOffsetX = fDistX - iCenterX * vTerrainScale.x;
	fOffsetX = vTerrainScale.x - fOffsetX;
	
	float fOffsetZ = fDistZ - iCenterZ * vTerrainScale.z;

	if (fOffsetZ > fOffsetX)
		iIdx = iIdx + 1;

	return iIdx;
}

void CTerrain::LoadTileTexture()
{
	CTexture* pTex = NULL;
	vector<CTexture*> vecTile;
	vector<CTexture*> vecTileNormal;

	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_01", L"Texture\\Tile\\Map\\Road_WNT_03_Diff.tga");
	vecTile.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_02", L"Texture\\Tile\\Map\\Rock_DST_01_Diff.tga");
	vecTile.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_03", L"Texture\\Tile\\Map\\Sand_DST_01_Diff.tga");
	vecTile.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_04", L"Texture\\Tile\\Blood_Diff.png");
	vecTile.push_back(pTex);
	
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_01_N", L"Texture\\Tile\\Map\\Road_WNT_03_Norm.tga");
	vecTileNormal.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_02_N", L"Texture\\Tile\\Map\\Rock_ALL_01_Norm.tga");
	vecTileNormal.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_03_N", L"Texture\\Tile\\Map\\Grass_DST_01_Norm.tga");
	vecTileNormal.push_back(pTex);
	pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"TILE_04_N", L"Texture\\Tile\\Road_ALL_10_Norm.tga");
	vecTileNormal.push_back(pTex);
	

	CTexture* pTileArr = CResMgr::GetInst()->CreateMultiTexture(L"TILE_ARR", vecTile, D3D11_USAGE_DEFAULT);
	CTexture* pTileNArr = CResMgr::GetInst()->CreateMultiTexture(L"TILE_N_ARR", vecTileNormal, D3D11_USAGE_DEFAULT);

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEXARR_0, &pTileArr);
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEXARR_1, &pTileNArr);

	// Brush Texture Load
	m_vecBrushTex.push_back((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Brush1", L"Texture\\Brush\\Brush_01.png"));
	m_vecBrushTex.push_back((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Brush2", L"Texture\\Brush\\Brush_02.png"));
	m_vecBrushTex.push_back((CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Circle", L"Texture\\Brush\\Circle.png"));
	m_iCurBrushIdx = 0;
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_2, &m_vecBrushTex[m_iCurBrushIdx]);

	CSceneMgr::GetInst()->SetTerrainObject(GetGameObject());
}

void CTerrain::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::TERRAIN;
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_eMod, sizeof(UINT), 1, _pFile);
	fwrite(&m_iCurBrushIdx, sizeof(UINT), 1, _pFile);
	fwrite(&m_iXFace, sizeof(UINT), 1, _pFile);
	fwrite(&m_iZFace, sizeof(UINT), 1, _pFile);
	fwrite(&m_fBrushScale, sizeof(float), 1, _pFile);
	fwrite(&m_fBrushRadius, sizeof(float), 1, _pFile);
	fwrite(&m_iPrevIdx, sizeof(UINT), 1, _pFile);
	fwrite(&m_bAway, sizeof(bool), 1, _pFile);
	fwrite(&m_iTileIdx, sizeof(int), 1, _pFile);
	fwrite(&m_bUseTess, sizeof(bool), 1, _pFile);

	void* pPixel = m_pHeightMapTex->GetPixel();
	fwrite(pPixel, sizeof(float), (size_t)(m_pHeightMapTex->GetHeight() * m_pWeightTex->GetWidth()), _pFile);
	pPixel = m_pWeightTex->GetPixel();
	fwrite(pPixel, sizeof(float), (size_t)(m_pHeightMapTex->GetHeight() * m_pWeightTex->GetWidth()), _pFile);

	//네비메시 세이브
	m_NaviMesh.Save(_pFile);
}

void CTerrain::Load(FILE * _pFile)
{
	fread(&m_eMod, sizeof(UINT), 1, _pFile);
	fread(&m_iCurBrushIdx, sizeof(UINT), 1, _pFile);
	fread(&m_iXFace, sizeof(UINT), 1, _pFile);
	fread(&m_iZFace, sizeof(UINT), 1, _pFile);
	fread(&m_fBrushScale, sizeof(float), 1, _pFile);
	fread(&m_fBrushRadius, sizeof(float), 1, _pFile);
	fread(&m_iPrevIdx, sizeof(UINT), 1, _pFile);
	fread(&m_bAway, sizeof(bool), 1, _pFile);
	fread(&m_iTileIdx, sizeof(int), 1, _pFile);
	fread(&m_bUseTess, sizeof(bool), 1, _pFile);

	CTexture* pTex = CResMgr::GetInst()->CreateTexture(L"HEIGHT_MAP", m_iXFace * 16, m_iZFace * 16, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R32_FLOAT);
	void* pPixel = pTex->GetPixel();
	fread(pPixel, sizeof(float), (size_t)(pTex->GetHeight() * pTex->GetWidth()), _pFile);
	pTex->Reset();
	SetHeightMap(pTex);

	pTex = CResMgr::GetInst()->CreateTexture(L"WEIGHT_TEX", m_iXFace * 16, m_iZFace * 16, D3D11_BIND_SHADER_RESOURCE, DXGI_FORMAT_R8G8B8A8_UNORM);
	pPixel = pTex->GetPixel();
	fread(pPixel, sizeof(float), (size_t)(pTex->GetHeight() * pTex->GetWidth()), _pFile);
	pTex->Reset();
	SetWeightMap(pTex);

	//네비메시 로드
	m_NaviMesh.Load(_pFile);
}
