#include "Text.h"

#include "Device.h"

#include "RenderMgr.h"
#include "Transform.h"

CText::CText()
	: m_pFW1Factory(NULL)
	, m_pFontWrapper(NULL)
	, m_fSize(0)
	, m_iColor(0)
	, m_arrColorInfo{}
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1Factory)))
	{
		assert(NULL);
	}

	if (FAILED(m_pFW1Factory->CreateFontWrapper(DEVICE, L"Arial", &m_pFontWrapper)))
	{
		assert(NULL);
	}
}

CText::~CText()
{
	SAFE_RELEASE(m_pFW1Factory);
	SAFE_RELEASE(m_pFontWrapper);
}

void CText::Awake()
{
}

int CText::Update()
{
	return 0;
}

void CText::Render()
{
	Vec2 vPos = GetWindowPosFromWorldPos();
	m_pFontWrapper->DrawString(
		CONTEXT,
		m_strText.c_str(),	// String
		m_fSize,			// Font size
		vPos.x,				// X position
		vPos.y,				// Y position
		m_iColor,			// Text color, 0xAaBbGgRr //ABGR(10, 240, 10, 255)
		0				    // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
	);
}

UINT CText::MakeColor(UINT _arrColorInfo[])
{
	return ABGR(_arrColorInfo[0], _arrColorInfo[1], _arrColorInfo[2], 255);
}

Vec2 CText::GetWindowPosFromWorldPos()
{
	Vec2 vWorldPos = Vec2(Transform()->GetWorldPos().x, Transform()->GetWorldPos().y);
	Vec2 vResolution = CRenderMgr::GetInst()->GetWindowResolution();

	Vec2 vRatio = Vec2(vResolution.x / WINSIZE_X, vResolution.y / WINSIZE_Y);
	vWorldPos = Vec2(vWorldPos.x * vRatio.x, vWorldPos.y * vRatio.y);

	Vec2 vWindowPos;
	vWindowPos.x = vWorldPos.x + (WINSIZE_X / 2.f);
	vWindowPos.y = (WINSIZE_Y / 2.f) - vWorldPos.y;

	return vWindowPos;
}

void CText::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::TEXT;
	fwrite(&iType, sizeof(UINT), 1, _pFile);
	fwrite(&m_fSize, sizeof(float), 1, _pFile);
	fwrite(&m_iColor, sizeof(wstring), 1, _pFile);
}

void CText::Load(FILE * _pFile)
{
	fread(&m_fSize, sizeof(float), 1, _pFile);
	fread(&m_iColor, sizeof(wstring), 1, _pFile);
}
