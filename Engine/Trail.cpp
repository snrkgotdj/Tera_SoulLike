#include "Trail.h"

#include "Device.h"

#include "TimeMgr.h"
#include "ResMgr.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"

#include "GameObject.h"
#include "Transform.h"
#include "MeshRender.h"

CTrail::CTrail()
	: m_pMesh(NULL)
	, m_iMaxCount(50)
	, m_fAddTime(0.f)
	, m_fCurTime(0.f)
	, m_arrToolObj{ NULL, NULL }
	, m_bActive(true)
	, m_fRenewTime(0.01f)
	, m_fDT(0.f)
{
	m_arrPos[0] = Vec3(0.f, 0.5f, 0.f);
	m_arrPos[1] = Vec3(0.f, -0.5f, 0.f);

	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	for (UINT i = 0; i < m_iMaxCount; ++i)
	{
		vecVtx.push_back(v);
		vecVtx.push_back(v);
	}

	UINT iCount = (UINT)((vecVtx.size() / 2 - 2) / 2);

	for (UINT i = 0; i < iCount; ++i)
	{
		vecIdx.push_back((i * 2));
		vecIdx.push_back((i * 2) + 3);
		vecIdx.push_back((i * 2) + 1);

		vecIdx.push_back((i * 2));
		vecIdx.push_back((i * 2) + 2);
		vecIdx.push_back((i * 2) + 3);
	}

	m_pMesh = CMesh::Create(vecVtx.size(), sizeof(VTX), D3D11_USAGE_DYNAMIC
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DYNAMIC, INDEX16::format(), &vecIdx[0]);

	m_pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_Trail");

	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Trail_01", L"Texture\\Trail\\Trail_03.png");

	pTex = NULL;
	m_pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &pTex);

	// 툴용 오브젝트
	CGameObject* pObj = NULL;

	for (int i = 0; i < 2; ++i)
	{
		pObj = new CGameObject;
		pObj->AddComponent<CTransform>(new CTransform);
		pObj->AddComponent<CMeshRender>(new CMeshRender);

		pObj->Transform()->SetLocalPos(m_arrPos[i]);
		pObj->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

		pObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
		pObj->MeshRender()->SetMaterial(m_pMtrl);

		m_arrToolObj[i] = pObj;
	}
}

CTrail::CTrail(const CTrail & _Other)
	: m_pMtrl(_Other.m_pMtrl)
	, m_listMat(_Other.m_listMat)
	, m_iMaxCount(_Other.m_iMaxCount)
	, m_fAddTime(_Other.m_fAddTime)
	, m_fCurTime(_Other.m_fCurTime)
	, m_fRenewTime(_Other.m_fRenewTime)
	, m_bActive(_Other.m_bActive)
{
	m_arrPos[0] = _Other.m_arrPos[0];
	m_arrPos[1] = _Other.m_arrPos[1];

	vector<VTX> vecVtx; VTX v;
	vector<WORD> vecIdx;

	for (UINT i = 0; i < m_iMaxCount; ++i)
	{
		vecVtx.push_back(v);
		vecVtx.push_back(v);
	}

	UINT iCount = (UINT)((vecVtx.size() / 2 - 2) / 2);

	for (UINT i = 0; i < iCount; ++i)
	{
		vecIdx.push_back((i * 2));
		vecIdx.push_back((i * 2) + 3);
		vecIdx.push_back((i * 2) + 1);

		vecIdx.push_back((i * 2));
		vecIdx.push_back((i * 2) + 2);
		vecIdx.push_back((i * 2) + 3);
	}

	m_pMesh = CMesh::Create(vecVtx.size(), sizeof(VTX), D3D11_USAGE_DYNAMIC
		, &vecVtx[0], vecIdx.size(), INDEX16::size()
		, D3D11_USAGE_DYNAMIC, INDEX16::format(), &vecIdx[0]);


	CGameObject* pObj = NULL;

	for (int i = 0; i < 2; ++i)
	{
		pObj = new CGameObject;
		pObj->AddComponent<CTransform>(new CTransform);
		pObj->AddComponent<CMeshRender>(new CMeshRender);

		pObj->Transform()->SetLocalPos(m_arrPos[i]);
		pObj->Transform()->SetLocalScale(Vec3(5.f, 5.f, 5.f));

		pObj->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
		pObj->MeshRender()->SetMaterial(m_pMtrl);

		m_arrToolObj[i] = pObj;
	}
}

CTrail::~CTrail()
{
	SAFE_DELETE(m_pMesh);

	for (int i = 0; i < 2; ++i)
	{
		SAFE_DELETE(m_arrToolObj[i]);
	}
}

void CTrail::Start()
{
}

int CTrail::Update()
{
	m_fCurTime += CTimeMgr::GetInst()->DeltaTime();
	m_fAddTime += CTimeMgr::GetInst()->DeltaTime();

	if (m_fAddTime >= m_fRenewTime)
	{
		m_listMat.push_front(Transform()->GetWorldMat());
		m_fAddTime = 0.f;
	}

	if (m_iMaxCount < m_listMat.size())
	{
		m_listMat.pop_back();
		m_fCurTime = 0.f;
	}

	m_fDT += CTimeMgr::GetInst()->DeltaTime();
	m_pMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &m_fDT);

	return 0;
}

int CTrail::DelayUpdate()
{
	m_fCurTime += CTimeMgr::GetInst()->DeltaTime();
	m_fAddTime += CTimeMgr::GetInst()->DeltaTime();

	if (m_fAddTime >= 0.01f)
	{
		m_listMat.push_front(Transform()->GetWorldMat());
		m_fAddTime = 0.f;
	}

	if (m_iMaxCount == m_listMat.size() * 0.5)
	{
		m_listMat.pop_back();
		m_fCurTime = 0.f;
	}

	for (int i = 0; i < 2; ++i)
	{
		m_arrToolObj[i]->Transform()->SetLocalPos(XMVector3TransformCoord(m_arrPos[i].Convert(), Transform()->GetWorldMat()));
		m_arrToolObj[i]->FinalUpdate();
	}

	return 0;
}

void CTrail::Render()
{
	if (false == m_bActive)
		return;

	vector<VTX> vecVtx; VTX v;

	v.vPos = XMVector3TransformCoord(m_arrPos[0].Convert(), Transform()->GetWorldMat());
	v.vUV = Vec2(0.f, 0.f);
	v.vColor = Vec4(0.2f, 0.8f, 0.2f, 1.f);
	vecVtx.push_back(v);

	v.vPos = XMVector3TransformCoord(m_arrPos[1].Convert(), Transform()->GetWorldMat());
	v.vUV = Vec2(0.f, 1.f);
	v.vColor = Vec4(0.2f, 0.8f, 0.2f, 1.f);
	vecVtx.push_back(v);

	UINT iIdx = 0;

	list<Matrix>::iterator iter = m_listMat.begin();
	for (; iter != m_listMat.end(); ++iter)
	{
		float fAlpha = (float)(m_iMaxCount - 1 - iIdx++) / (float)m_iMaxCount;
		fAlpha *= 0.8f;

		v.vPos = XMVector3TransformCoord(m_arrPos[0].Convert(), (*iter));
		v.vUV = Vec2((float)iIdx/ ((float)m_iMaxCount * 0.5f - 1), 0.f);
		v.vColor = Vec4(0.2f, 0.8f, 0.2f, fAlpha);
		vecVtx.push_back(v);

		v.vPos = XMVector3TransformCoord(m_arrPos[1].Convert(), (*iter));
		v.vUV = Vec2((float)iIdx / ((float)m_iMaxCount * 0.5f - 1), 1.f);
		v.vColor = Vec4(0.2f, 0.8f, 0.2f, fAlpha);
		vecVtx.push_back(v);
	}

	m_pMesh->ResetVTX(vecVtx);

	Transform()->UpdateData(XMMatrixIdentity());
	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_NONE);
	m_pMtrl->UpdateData();
	m_pMesh->SetLayout(m_pMtrl->GetShader()->GetVSBlob());
	m_pMesh->UpdateData();
	m_pMesh->Render(0);
}

void CTrail::ToolRender()
{
	if (false == m_bActive)
		return;

	for (int i = 0; i < 2; ++i)
	{
		m_arrToolObj[i]->Render();
	}
}

void CTrail::SetTrailTexture(CTexture * _pTrailTex)
{
	m_pMtrl->SetData(SHADER_PARAM::SP_TEX_1, &_pTrailTex);
}

void CTrail::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::TRAIL;
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_arrPos, sizeof(Vec3), 2, _pFile);
	fwrite(&m_iMaxCount, sizeof(int), 1, _pFile);
	fwrite(&m_fAddTime, sizeof(float), 1, _pFile);
	fwrite(&m_fCurTime, sizeof(float), 1, _pFile);
}

void CTrail::Load(FILE * _pFile)
{
	fread(&m_arrPos, sizeof(Vec3), 2, _pFile);
	fread(&m_iMaxCount, sizeof(int), 1, _pFile);
	fread(&m_fAddTime, sizeof(float), 1, _pFile);
	fread(&m_fCurTime, sizeof(float), 1, _pFile);
}