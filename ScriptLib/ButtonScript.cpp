#include "ButtonScript.h"
#include "MgrScript\UIMgrScript.h"

CButtonScript::CButtonScript()
	: m_eOnClick(NULL)
	, m_pMtrl(NULL)
	, m_eState(BUTTON_STATE::NONE)
{
}

CButtonScript::~CButtonScript()
{
}

void CButtonScript::Awake()
{
	CUIMgrScript::GetInst()->AddButton(this);
}

int CButtonScript::Update()
{
	int iData = (int)m_eState;
	m_pMtrl->SetData(SHADER_PARAM::SP_INT_0, &iData);

	if (BUTTON_STATE::CLICK == m_eState &&
		CKeyMgr::GetInst()->GetKeyState(KEY_TYPE::KEY_LBTN, KEY_STATE::STATE_AWAY))
	{
		m_eState = BUTTON_STATE::NONE;
	}

	return 0;
}

void CButtonScript::Init()
{
	m_pMtrl = MeshRender()->GetUIMaterial();
}

void CButtonScript::SetOnClick(Event _Function)
{
	m_eOnClick = std::move(_Function);
}

void CButtonScript::OnClick()
{
	if (NULL != m_eOnClick)
		m_eOnClick();
}

void CButtonScript::SetState(const BUTTON_STATE & _eState)
{
	if (BUTTON_STATE::CLICK != m_eState)
	{
		m_eState = _eState;
	}
}

void CButtonScript::Save(FILE * _pFile)
{
	wstring strKey;

	void* pData = m_pMtrl->GetData(SHADER_PARAM::SP_TEX_0);
	CTexture* pTex = *((CTexture**)pData);

	if (NULL != pTex)
		strKey = pTex->GetKey();

	SaveWString(strKey, _pFile);
}

void CButtonScript::Load(FILE * _pFile)
{
	Init();
	wstring strKey = LoadWString(_pFile);

	if (!strKey.empty())
	{
		CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);
		m_pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
	}
}