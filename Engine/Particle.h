#pragma once
#include "Component.h"

class CShader;
class CMaterial;


struct tPARTICLE
{
	Vec3 vPos;
	Vec2 vSize;
	Vec3 vDir;
	float fTime;
	PARTICLE_TYPE eType;
};

class CParticle :
	public CComponent
{
private:
	ID3D11Buffer* m_pInitVB;
	ID3D11Buffer* m_pDrawVB;
	ID3D11Buffer* m_pStreamVB;

	ID3D11Buffer* m_pIdxBuffer;

	ID3D11DepthStencilState*	m_pDisableDepth;

	UINT m_pIdxCnt;

	UINT m_iVtxSize;
	vector<D3D11_INPUT_ELEMENT_DESC> m_vecLayoutDesc;
	
	ID3D11InputLayout* m_pLayout;
	UINT			   m_iLayoutOffset;
	
	ID3DBlob* m_pRecentBlob;

	CMaterial* m_pStreamMtrl;
	CMaterial* m_pDrawMtrl;

	bool m_bFirstRun;

public:
	void SetLayout(ID3DBlob * _pVSBlob);
	void CreateSOB(UINT _iSize = 100);

private:
	void AddLayoutDesc(char* _pSemanticName, int _iSemanticIdx, DXGI_FORMAT _eFormat
		, int _iInputSlot, int _iInstancing, D3D11_INPUT_CLASSIFICATION _eClass = D3D11_INPUT_PER_VERTEX_DATA);

public:
	virtual void Awake();
	virtual void Start();
	virtual int Update();
	virtual int DelayUpdate();

	void Render();

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);

public:
	CLONE(CParticle);

public:
	CParticle();
	virtual ~CParticle();
};

