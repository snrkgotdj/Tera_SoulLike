#include "ElectricEffect.h"

CElectricEffect::CElectricEffect()
{
	m_ID = SCRIPT_ELECTRIC_EFFECT;
}

CElectricEffect::~CElectricEffect()
{
}

int CElectricEffect::Update()
{
	if (false == m_bStart)
		return 0;

	static float fData = 0;
	fData += DT();
	MeshRender()->GetSharedMaterial()->SetData(SHADER_PARAM::SP_FLOAT_0, &fData);

	return 0;
}