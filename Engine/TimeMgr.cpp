#include "TimeMgr.h"

#include "Device.h"

CTimeMgr::CTimeMgr()
	: m_llFrequency{0}
	, m_llCurCnt{0}
	, m_llOldCnt{0}
	, m_pFW1Factory(NULL)
	, m_pFontWrapper(NULL)
	, m_fDeltaTime(0.f)
	, m_fOldDeltaTime(1.f)
	, m_fAccTime(0.f)
	, m_iAccFrame(0)
	, m_fTimer(0)
{
}

CTimeMgr::~CTimeMgr()
{
	SAFE_RELEASE(m_pFW1Factory);
	SAFE_RELEASE(m_pFontWrapper);
}

void CTimeMgr::init()
{


	// 초당 진동 회수
	QueryPerformanceFrequency(&m_llFrequency);
	QueryPerformanceCounter(&m_llCurCnt);
	m_llOldCnt = m_llCurCnt;

	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
	{
		assert(NULL);
	}

	if (FAILED(m_pFW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_pFontWrapper)))
	{
		assert(NULL);
	}
}

int CTimeMgr::Update()
{
	int iRet = 0;

	QueryPerformanceCounter(&m_llCurCnt);

	if (m_llCurCnt.QuadPart == m_llOldCnt.QuadPart)
		return false;


	m_fDeltaTime = (m_llCurCnt.QuadPart - m_llOldCnt.QuadPart) / (float)m_llFrequency.QuadPart;
	m_llOldCnt = m_llCurCnt;


	// DT 가 너무 크게 튀는경우
	if (m_fDeltaTime > m_fOldDeltaTime * 1000.f)
	{
		m_fDeltaTime = m_fOldDeltaTime;
	}	

	// FPS 계산
	m_fFPS = 1 / m_fDeltaTime;
	m_iAccFrame += 1;
	
	m_fAccTime += m_fDeltaTime;
	if (m_fAccTime > 1.f)
	{
		m_fAccTime = 0.f;
		QueryPerformanceFrequency(&m_llFrequency);

		// 프레임 카운팅 0 으로 초기화
		m_fAccFPS = (float)m_iAccFrame;
		m_iAccFrame = 0;
	}

	m_fOldDeltaTime = m_fDeltaTime;

	m_fFrameLimit += m_fDeltaTime;
	if (m_fFrameLimit > 0.017)
	{
		m_fFrameLimit = 0;
		//if (m_fDeltaTime <= 0.016666)
			m_fDeltaTime = 0.016666f;

		iRet = 1;
		// 타이머
		m_fTimer += m_fDeltaTime;
		if (m_fTimer > 10000.f)		
			m_fTimer = 0.f;	
	}
	

	g_Transform.g_fDelta.x = m_fDeltaTime;
	g_Transform.g_fDelta.y = m_fTimer * 0.4f;

	return iRet;
}

void CTimeMgr::render(HDC _dc)
{
	wchar_t szFPS[50] = {};
	wsprintf(szFPS, L"FPS : %d", (int)m_fAccFPS);
	//TextOut(_dc, 0, 0, szFPS, wcslen(szFPS));

	m_pFontWrapper->DrawString(
		CONTEXT,
		szFPS,
		50.f,			
		30.f,			
		65.f,
		ABGR(10, 240, 10, 255),
		0				
	);
}
