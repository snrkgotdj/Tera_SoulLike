#include "Core.h"

#include "Device.h"
#include "pathmgr.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "ShaderMgr.h"
#include "ResMgr.h"
#include "SceneMgr.h"
#include "CollisionMgr.h"
#include "InstancingBuffer.h"
#include "Sound.h"
#include "RenderMgr.h"


CCore::CCore()
	: m_hWnd(NULL)
{

}

CCore::~CCore()
{	
	CSceneMgr::GetInst()->DistroyInst();
	CRenderMgr::GetInst()->DistroyInst();
	CResMgr::GetInst()->DistroyInst();
}

int CCore::init(HWND _hWnd, bool _bWindow)
{
	m_hWnd = _hWnd;

	if (FAILED(CDevice::GetInst()->init(_hWnd, _bWindow)))
	{	
		MessageBox(_hWnd, L"장치 초기화 실패", L"실패", MB_OK);		
		return RET_FAILED;
	}

	CreateConstBuffer();

	// Manager 초기화
	CPathMgr::Init();
	CKeyMgr::GetInst()->init();
	CTimeMgr::GetInst()->init();
	CShaderMgr::GetInst()->init();
	CResMgr::GetInst()->init();
	CSceneMgr::GetInst()->Init();
	CCollisionMgr::GetInst()->Init();
	CInstancingBuffer::GetInst()->init();

	return RET_SUCCESS;
}

int CCore::run()
{
	if (CTimeMgr::GetInst()->Update())
	{
		Update();

		Render();
	}
	return 0;
}

int CCore::Update()
{
	CKeyMgr::GetInst()->Update();
	
	CSceneMgr::GetInst()->Progress();
	
	return 0;
}

void CCore::Render()
{
	CSceneMgr::GetInst()->Render();	
}

void CCore::CreateConstBuffer()
{
	// 상수버퍼 만들기
	CDevice::GetInst()->CreateConstBuffer(L"Transform", sizeof(tTransform), D3D11_USAGE_DYNAMIC, 0);

	// Int
	CDevice::GetInst()->CreateConstBuffer
		( g_SPName[(UINT)SHADER_PARAM::SP_INT_END]
		, sizeof(int) *((UINT) SHADER_PARAM::SP_INT_END - (UINT)SHADER_PARAM::SP_INT_0)
		, D3D11_USAGE_DYNAMIC
		, g_SPRegister[(UINT)SHADER_PARAM::SP_INT_0]);

	// Float
	CDevice::GetInst()->CreateConstBuffer
		( g_SPName[(UINT)SHADER_PARAM::SP_FLOAT_END]
		, sizeof(float) *((UINT)SHADER_PARAM::SP_FLOAT_END - (UINT)SHADER_PARAM::SP_FLOAT_0)
		, D3D11_USAGE_DYNAMIC
		, g_SPRegister[(UINT)SHADER_PARAM::SP_FLOAT_0]);

	// Vec4
	CDevice::GetInst()->CreateConstBuffer
		(g_SPName[(UINT)SHADER_PARAM::SP_VEC4_END]
			, sizeof(Vec4) *((UINT)SHADER_PARAM::SP_VEC4_END - (UINT)SHADER_PARAM::SP_VEC4_0)
			, D3D11_USAGE_DYNAMIC
			, g_SPRegister[(UINT)SHADER_PARAM::SP_VEC4_0]);

	// Animation Info Buffer
	CDevice::GetInst()->CreateConstBuffer
		( L"AnimationInfo"
			, sizeof(tAnimInfo)
			, D3D11_USAGE_DYNAMIC
			, 13);

	// Light Buffer
	CDevice::GetInst()->CreateConstBuffer
	(L"LightArr"
		, sizeof(tLightArr)
		, D3D11_USAGE_DYNAMIC
		, 4);

	// TexCheck Buffer
	CDevice::GetInst()->CreateConstBuffer
	(L"TexCheck"
		, sizeof(UINT) * ((UINT)SHADER_PARAM::SP_TEX_END - (UINT)SHADER_PARAM::SP_TEX_0)
		, D3D11_USAGE_DYNAMIC
		, 5);
}
