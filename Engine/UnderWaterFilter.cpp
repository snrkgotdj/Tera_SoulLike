#include "UnderWaterFilter.h"
#include "TimeMgr.h"
#include "Material.h"

CUnderWaterFilter::CUnderWaterFilter()
{
}

CUnderWaterFilter::~CUnderWaterFilter()
{
}

int CUnderWaterFilter::FinalUpdate()
{
	CCamFilter::FinalUpdate();

	float fTimer =CTimeMgr::GetInst()->GetTimer() * 0.3f;
	m_pMtrl->SetData(SHADER_PARAM::SP_FLOAT_0, &fTimer);

	return 0;
}
