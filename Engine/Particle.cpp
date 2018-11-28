#include "Particle.h"
#include "Device.h"
#include "Shader.h"
#include "ShaderMgr.h"
#include "ResMgr.h"
#include "TimeMgr.h"
#include "SceneMgr.h"
#include "Material.h"
#include "Shader.h"

#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

CParticle::CParticle()
	: m_pInitVB(nullptr)
	, m_pDrawVB(nullptr)
	, m_pStreamVB(nullptr)
	, m_pIdxBuffer(nullptr)
	, m_pLayout(nullptr)
	, m_pRecentBlob(nullptr)
	, m_pStreamMtrl(nullptr)
	, m_pDrawMtrl(nullptr)
	, m_bFirstRun(true)
	, m_pIdxCnt(0)
{
}

CParticle::~CParticle()
{
	SAFE_RELEASE(m_pInitVB);
	SAFE_RELEASE(m_pDrawVB);
	SAFE_RELEASE(m_pStreamVB);
	SAFE_RELEASE(m_pLayout);
	SAFE_RELEASE(m_pIdxBuffer);
	SAFE_RELEASE(m_pDisableDepth);
}

void CParticle::CreateSOB(UINT _iSize)
{
	//스트림 버퍼 생성
	D3D11_BUFFER_DESC tDesc = {};
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.ByteWidth = sizeof(tPARTICLE) * 1;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;
	tDesc.StructureByteStride = 0;

	m_iVtxSize = sizeof(tPARTICLE);

	tPARTICLE tParticle;
	memset(&tParticle, 0, sizeof(tPARTICLE));

	tParticle.vPos = Vec3(20.f, 0.f, 0.f);
	tParticle.vSize = Vec2(10.f, 10.f);
	tParticle.vDir = Vec3(0.f, 1.f, 0.f);
	tParticle.fTime = 0;
	tParticle.eType = PARTICLE_TYPE::Emiter;

	D3D11_SUBRESOURCE_DATA tData = {};
	tData.pSysMem = &tParticle;
	
	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tData, &m_pInitVB)))
	{
		assert(NULL);
	}

	tDesc.ByteWidth = sizeof(tPARTICLE) * _iSize;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, 0, &m_pDrawVB)))
	{
		assert(NULL);
	}
	if (FAILED(DEVICE->CreateBuffer(&tDesc, 0, &m_pStreamVB)))
	{
		assert(NULL);
	}

	AddLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddLayoutDesc("SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	AddLayoutDesc("VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	AddLayoutDesc("AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 0);
	AddLayoutDesc("TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 0);

	//메테리얼 설정
	m_pStreamMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_StreamOut");
	m_pDrawMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_DrawParticle");

	//인덱스 버퍼 생성
	vector<WORD> vecIdx;
	vecIdx.push_back(0); vecIdx.push_back(1); vecIdx.push_back(2);vecIdx.push_back(3);

	m_pIdxBuffer = CDevice::GetInst()->CreateIdxBuffer(vecIdx.size(), INDEX16::size(), &vecIdx[0]);
	m_pIdxCnt = vecIdx.size();

	D3D11_DEPTH_STENCIL_DESC tDepth = {};

	tDepth.DepthEnable = false;
	tDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	HRESULT hr = DEVICE->CreateDepthStencilState(&tDepth, &m_pDisableDepth);

	int a = 0;
}

void CParticle::AddLayoutDesc(char * _pSemanticName, int _iSemanticIdx, DXGI_FORMAT _eFormat, int _iInputSlot, int _iInstancing, D3D11_INPUT_CLASSIFICATION _eClass)
{
	D3D11_INPUT_ELEMENT_DESC tDesc = {};

	tDesc.SemanticName = _pSemanticName;
	tDesc.SemanticIndex = _iSemanticIdx; // 같은 종류의 시멘틱에 대해서 증가하는 인덱스
	tDesc.Format = _eFormat;
	tDesc.InputSlot = _iInputSlot; // 0 ~ 15 사이의 값
	tDesc.InputSlotClass = _eClass;
	tDesc.InstanceDataStepRate = _iInstancing; // 인스턴싱

	tDesc.AlignedByteOffset = m_iLayoutOffset;
	m_iLayoutOffset += GetSizeofFormat(_eFormat);

	m_vecLayoutDesc.push_back(tDesc);
}

void CParticle::Awake()
{

}

void CParticle::Start()
{
}

int CParticle::Update()
{
	
	return 0;
}

void CParticle::SetLayout(ID3DBlob * _pVSBlob)
{
	if (m_pRecentBlob == _pVSBlob)
		return;

	m_pRecentBlob = _pVSBlob;

	SAFE_RELEASE(m_pLayout);

	FAILED(DEVICE->CreateInputLayout(&m_vecLayoutDesc[0], m_vecLayoutDesc.size()
		, m_pRecentBlob->GetBufferPointer()
		, m_pRecentBlob->GetBufferSize()
		, &m_pLayout));

}

int CParticle::DelayUpdate()
{
	return 0;
}

void CParticle::Render()
{
	// 스트림 출력 변수세팅
	// 델타타임, 랜덤Vec3, EmitPos
	float fDelta = CTimeMgr::GetInst()->DeltaTime();
	Vec3 vRandom = getRandNum_Vec3();
	Vec3 vEmitPos = Transform()->GetWorldPos();
	
	m_pStreamMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &fDelta);
	m_pStreamMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vRandom);
	m_pStreamMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vEmitPos);
	
	SetLayout(m_pStreamMtrl->GetShader()->GetVSBlob());
	CONTEXT->IASetInputLayout(m_pLayout);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	m_pStreamMtrl->UpdateData();
	
	UINT iOffset = 0;
	UINT iStride = m_iVtxSize;
	
	CONTEXT->OMSetDepthStencilState(m_pDisableDepth, 0);
	CONTEXT->IASetVertexBuffers(0, 1, &m_pInitVB, &iStride, &iOffset);
	CONTEXT->SOSetTargets(1, &m_pStreamVB, &iOffset);
	CONTEXT->Draw(1, 0);

	// DrawParticle 변수 세팅
	// vCamAxisX, vCamAxisY
	CGameObject* pCamera = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
	Vec4 vCamAxisX = pCamera->Transform()->GetDir(DIR_TYPE::RIGHT).GetVec4();
	Vec4 vCamAxisY = pCamera->Transform()->GetDir(DIR_TYPE::UP).GetVec4();
	
	m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vCamAxisX);
	m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vCamAxisY);
	
	ID3D11Buffer* bufferArray[1] = { 0 };
	SetLayout(m_pDrawMtrl->GetShader()->GetVSBlob());
	CONTEXT->SOSetTargets(1, bufferArray, &iOffset);

	m_pDrawMtrl->UpdateData();
	CONTEXT->IASetInputLayout(m_pLayout);
	CONTEXT->IASetVertexBuffers(0, 1, &m_pStreamVB, &iStride, &iOffset);
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	CONTEXT->IASetIndexBuffer(m_pIdxBuffer, INDEX16::format(), 0);
	CONTEXT->DrawIndexed(4, 0, 0);
	//CONTEXT->DrawAuto();


	//CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_FRONT);
	//
	//// DrawParticle 변수 세팅
	//// vCamAxisX, vCamAxisY
	//CGameObject* pCamera = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::TOOL);
	//Vec4 vCamAxisX = pCamera->Transform()->GetDir(DIR_TYPE::RIGHT).GetVec4();
	//Vec4 vCamAxisY = pCamera->Transform()->GetDir(DIR_TYPE::UP).GetVec4();
	//
	//m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vCamAxisX);
	//m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vCamAxisY);
	//
	//m_pDrawMtrl->UpdateData();
	//SetLayout(m_pDrawMtrl->GetShader()->GetVSBlob());

	//UINT iOffset = 0;
	//UINT iStride = m_iVtxSize;

	//CONTEXT->IASetInputLayout(m_pLayout);
	//CONTEXT->IASetVertexBuffers(0, 1, &m_pInitVB, &iStride, &iOffset);
	//CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//CONTEXT->SOSetTargets(1, &m_pStreamVB, &iOffset);

	//CONTEXT->IASetIndexBuffer(m_pIdxBuffer, INDEX16::format(), 0);
	//CONTEXT->DrawIndexed(4, 0, 0);
	////CONTEXT->DrawAuto();

	//CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
}

void CParticle::Save(FILE * _pFile)
{
}

void CParticle::Load(FILE * _pFile)
{
}



//// 스트림 출력 변수세팅
//// 델타타임, 랜덤Vec3, EmitPos
//float fDelta = CTimeMgr::GetInst()->DeltaTime();
//Vec3 vRandom = getRandNum_Vec3();
//Vec3 vEmitPos = Transform()->GetWorldPos();
//
//m_pStreamMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &fDelta);
//m_pStreamMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vRandom);
//m_pStreamMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vEmitPos);
//
//SetLayout(m_pStreamMtrl->GetShader()->GetVSBlob());
//CONTEXT->IASetInputLayout(m_pLayout);
//CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//
//m_pStreamMtrl->UpdateData();
//
//UINT iOffset = 0;
//UINT iStride = m_iVtxSize;
//
//if (m_bFirstRun)
//CONTEXT->IASetVertexBuffers(0, 1, &m_pInitVB, &iStride, &iOffset);
//else
//CONTEXT->IASetVertexBuffers(0, 1, &m_pDrawVB, &iStride, &iOffset);
//
//CONTEXT->SOSetTargets(1, &m_pStreamVB, &iOffset);
//CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_NONE);
//
//if (m_bFirstRun == true)
//{
//	CONTEXT->DrawIndexed(4, 0, 0);
//	m_bFirstRun = false;
//}
//else
//{
//	CONTEXT->DrawAuto();
//}
//
//// DrawParticle 변수 세팅
//// vCamAxisX, vCamAxisY
//CGameObject* pCamera = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN);
//Vec4 vCamAxisX = pCamera->Transform()->GetDir(DIR_TYPE::RIGHT).GetVec4();
//Vec4 vCamAxisY = pCamera->Transform()->GetDir(DIR_TYPE::UP).GetVec4();
//
//m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vCamAxisX);
//m_pDrawMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vCamAxisY);
//
////// 쉐이더 스왑
//ID3D11Buffer* bufferArray[1] = { 0 };
//SetLayout(m_pDrawMtrl->GetShader()->GetVSBlob());
//CONTEXT->SOSetTargets(1, bufferArray, &iOffset);
//
//std::swap(m_pDrawVB, m_pStreamVB);
//CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_NONE);
//
//m_pDrawMtrl->UpdateData();
//CONTEXT->IASetInputLayout(m_pLayout);
//CONTEXT->IASetVertexBuffers(0, 1, &m_pDrawVB, &iStride, &iOffset);
//CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//
//CONTEXT->IASetIndexBuffer(m_pIdxBuffer, INDEX16::format(), 0);
//CONTEXT->DrawIndexed(4, 0, 0);
////CONTEXT->DrawAuto();