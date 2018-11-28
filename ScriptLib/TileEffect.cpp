#include "TileEffect.h"

CTileEffect::CTileEffect()
	: m_fLiveTime(0.f)
	, m_fTime(0.f)
	, m_eType(TILE_TYPE::END)
{
	m_ID = SCRIPT_TILE;
}

CTileEffect::~CTileEffect()
{
}

int CTileEffect::Update()
{
	m_fLiveTime += DT();

	if (m_fTime <= m_fLiveTime)
	{
		CResMgr::GetInst()->DeleteCloneMtrl(MeshRender()->GetSharedMaterial());
		GetGameObject()->Destroy();
	}
	else
	{
		float fData = m_fLiveTime / m_fTime;
		MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);
	}

	return 0;
}

void CTileEffect::SetTile(TILE_TYPE _eType)
{
	m_eType = _eType;

	CTexture* pTex = NULL;
	switch (_eType)
	{
	case TILE_TYPE::BLOOD:
		pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Blood_1");
		break;
	case TILE_TYPE::CRACK:
		pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Crack");
		break;
	case TILE_TYPE::CRACK2:
		pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Crack2");
		break;
	case TILE_TYPE::END:
		break;
	default:
		break;
	}

	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
}