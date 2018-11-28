#include "stdafx.h"
#include "ToolUIScript.h"

#define UI_SIZE_X WINSIZE_X / 10
#define UI_SIZE_Y WINSIZE_Y / 10

CToolUIScript::CToolUIScript()
{
}


CToolUIScript::~CToolUIScript()
{
}

void CToolUIScript::Start()
{
	m_matProj = XMMatrixOrthographicLH(WINSIZE_X, WINSIZE_Y, 1, 100);
	CMaterial* pMtrl = MeshRender()->GetSharedMaterial();

	Vec4 vM_0(m_matProj._11, m_matProj._12, m_matProj._13, m_matProj._14);
	Vec4 vM_1(m_matProj._21, m_matProj._22, m_matProj._23, m_matProj._24);
	Vec4 vM_2(m_matProj._31, m_matProj._32, m_matProj._33, m_matProj._34);
	Vec4 vM_3(m_matProj._41, m_matProj._42, m_matProj._43, m_matProj._44);

	pMtrl->SetData(SHADER_PARAM::SP_VEC4_0, &vM_0);
	pMtrl->SetData(SHADER_PARAM::SP_VEC4_1, &vM_1);
	pMtrl->SetData(SHADER_PARAM::SP_VEC4_2, &vM_2);
	pMtrl->SetData(SHADER_PARAM::SP_VEC4_3, &vM_3);
}

int CToolUIScript::Update()
{
	return 0;
}

void CToolUIScript::SetUINumber(UINT _i)
{
	Transform()->SetLocalScale(Vec3(UI_SIZE_X, UI_SIZE_Y, 1.f));
	Transform()->SetLocalPos(
		Vec3(-WINSIZE_X / 2.f + UI_SIZE_X / 2.f + UI_SIZE_X * (_i / 8) + 5 * (_i / 8)
			, WINSIZE_Y / 2.f - UI_SIZE_Y / 2.f - UI_SIZE_Y * (_i % 8) - 5 * (_i % 8), 1.f));

	CMaterial* pMtrl = MeshRender()->GetMaterial();

	wstring strTexKey = L"RenderTarget_";
	strTexKey += to_wstring(_i + 1);

	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strTexKey);
	pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);

	if (nullptr == pTex)
		assert(NULL);
}
