#pragma once
#include "Component.h"

#include "ResPtr.h"
#include "NaviMesh.h"

class CTexture;

enum class TERRAIN_MOD
{
	HEIGHT_MOD,
	HEIGHTMAP_MOD,
	HEIGHT_RESET_MOD,
	HEIGHTMAP_REVERSE_MOD,
	HEIGHTMAP_SPLAT_MOD,
	SPLAT_MOD,
	NAVIMESH_MOD,
	END,
};

class CTerrain :
	public CComponent
{
private:
	TERRAIN_MOD					m_eMod;
	CResPtr<CTexture>			m_pHeightMapTex;
	CResPtr<CTexture>			m_pWeightTex;
	vector<CResPtr<CTexture>>   m_vecBrushTex;
	UINT						m_iCurBrushIdx;

	CNaviMesh					m_NaviMesh;
	int							m_iCurPickPatch;

	// Terrain Face Count
	UINT						m_iXFace;
	UINT						m_iZFace;

	float						m_fBrushScale;
	float						m_fBrushRadius;

	UINT						m_iPrevIdx;
	bool						m_bAway;
	bool						m_bUseTess;
	int							m_iTileIdx;

public:
	virtual void Awake();
	virtual void Start();
	virtual int Update();
	virtual int DelayUpdate();

public:
	void Init();
	void SetHeightMap(CTexture* _pHeightTex);
	void SetWeightMap(CTexture* _pWeightTex);
	void SetFaceCount(UINT _iXFace, UINT _iZFace);
	void LoadTileTexture();
	void SetMod(const TERRAIN_MOD& _eMod) { m_eMod = _eMod; }
	const TERRAIN_MOD& GetMod() { return m_eMod; }
	void SetScale(float _fScale) { m_fBrushScale = _fScale; }
	void SetTessUse(bool _bTessUse) { m_bUseTess = _bTessUse; }
	void SetRadiuse(float _fRadius) { m_fBrushRadius = _fRadius; }
	float GetRadiuse() { return m_fBrushRadius; }
	UINT GetCurPatch() { return m_iCurPickPatch; }

	void SetBrush(int _iIdx);
	void SetTileIdx(int _iIdx) { m_iTileIdx = _iIdx; }
	int GetWorldPosbyTex(Vec3& _vPos);
	Vec3 GetTerrainPos(const Vec3& _vPos);
	CNaviMesh* GetNaviMesh() { return &m_NaviMesh; }
	UINT GetPatchIdxByPos(Vec3& _vPos);
	int Picking(Vec3& _vPos = Vec3());

	void SetBlood(Vec3& _vPos);

private:
	void KeyCheck();
	void EditHeight(int _iPatchIdx);
	void EditHeightReverse(int _iPatchIdx);
	void EditHeightReset(int _iPatchIdx);
	void EditHeightReverse2(int _iPatchIdx);
	void EditHeightSplat(int _iPatchIdx);
	void ResetHeight();
	void EditWeight(int _iPatchIdx);
	void EditWeightAll();
	void EditWeightBlood(int _iPatchIdx);
	void EditNaviMesh(int _iPatchIdx);

	void GetBrushRatio(UINT _iPatchIdx, Vec2& _vBrushScale, Vec2& _vBrushPos);
	void BrushUpdate(UINT _iPatchIdx);
	void CheckAllNavi();


public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CLONE(CTerrain);

public:
	CTerrain();
	virtual ~CTerrain();
};