#pragma once
#include "ResPtr.h"
#include "FBXLoader.h"

class CShader;
class CTexture;

struct tIndexInfo
{
	ID3D11Buffer*	pIB;
	UINT			iIdxCount;
	UINT			iIdxSize;
	D3D11_USAGE		eIdxUsage;
	DXGI_FORMAT		eIdxFormat;
	void*			pIdxSysMem;
};

class CMesh :
	public CRes
{
private:
	ID3D11Buffer*						m_pVB;
	UINT								m_iVtxCount;
	UINT								m_iVtxSize;
	D3D11_USAGE							m_eVtxUsage;
	void*								m_pVtxSysMem;

	vector<tIndexInfo>					m_vecIdxInfo;

	D3D11_PRIMITIVE_TOPOLOGY			m_eTopology;
		
	// Input Layout
	ID3D11InputLayout*					m_pLayout;
	vector<D3D11_INPUT_ELEMENT_DESC>	m_vecLayoutDesc;
	UINT								m_iLayoutOffset;
	UINT								m_iInstancingOffset;
	ID3DBlob*							m_pRecentBlob;

	// Animation
	vector<tMTAnimClip>					m_vecAnimClip;
	vector<tMTBone>						m_vecBones;
	CResPtr<CTexture>					m_pBoneTex;

public:
	static CMesh* Create(UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _eVtxUsage, void* _pVtxSysMem
		, UINT _iIdxCount, UINT _iIdxSize, D3D11_USAGE _eIdxUsage, DXGI_FORMAT _eIdxFormat, void* _pIdxSysMem
		, D3D11_PRIMITIVE_TOPOLOGY _eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static CMesh* Create(UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _eVtxUsage, void* _pVtxSysMem
		, vector<tIndexInfo>& _IdxInfo, D3D11_PRIMITIVE_TOPOLOGY _eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static CMesh* CreateFromContainer(CFBXLoader& _loader);

	void ResetVTX(VTX* _vecVTX, UINT _iVtxCount);
	void ResetVTX(vector<VTX>& _vecVTX);
	void Reset(vector<VTX>& _vecVTX, vector<WORD>& _vecIDX);

	void SetVtxSysMem(void* _pVtxSysMem, UINT _iVtxSize, UINT _iVtxCount, bool _bDeleteMem = true);
	void SetIdxSysMem(void* _pIdxSysMem, UINT _iIdxSize, UINT _iIdxCount, UINT _iIdxNumber);
	VTX* GetVtxSysMem() { return (VTX*)m_pVtxSysMem; };
	void* GetIdxSysMem(UINT _iIdx = 0) { return m_vecIdxInfo[_iIdx].pIdxSysMem; }
	UINT GetIdxCount(UINT _iIdx = 0) { return m_vecIdxInfo[_iIdx].iIdxCount; }
	UINT GetVtxCount() { return m_iVtxCount; }
	UINT GetVtxSize() { return m_iVtxSize; }
	UINT GetSubmeshCount() { return (UINT)m_vecIdxInfo.size(); }
	D3D11_USAGE GetVtxUsage() { return m_eVtxUsage; };
	D3D11_PRIMITIVE_TOPOLOGY GetTopology() { return m_eTopology; }
	ID3D11Buffer* GetVB() { return m_pVB; }
	const tIndexInfo& GetIdxInfo(UINT _i) { return m_vecIdxInfo[_i]; };
	const vector<tMTBone>* GetBones() { return &m_vecBones; }
	const vector<tMTAnimClip>* GetAnimClip() { return &m_vecAnimClip; }
	void SetBoneTex(CTexture* _pTex) { m_pBoneTex = _pTex; }
	CTexture* GetBoneTex() { return m_pBoneTex; }

	void AddLayoutDesc(char* _pSemanticName, int _iSemanticIdx, DXGI_FORMAT _eFormat
		, int _iInputSlot, int _iInstancing, D3D11_INPUT_CLASSIFICATION _eClass = D3D11_INPUT_PER_VERTEX_DATA);
	int SetLayout(ID3DBlob* _pVSBlob);	

	virtual void UpdateData();
	void UpdateData(D3D11_PRIMITIVE_TOPOLOGY _eTopology);
	void UpdateDataInstancing();
	void Render(UINT _iIdx = 0);
	void renderInstancing(UINT _iIdx = 0);

public:
	virtual void Save(FILE* _pFile);
	static CMesh* Load(FILE* _pFile);

public:
	CMesh();
	virtual ~CMesh();
};

