#include "Audio.h"

#include "ResMgr.h"
#include "Transform.h"

CAudio::CAudio()
	: m_pChannel(NULL)
	, m_eType(SOUND_TYPE::TYPE_2D)
{
}

CAudio::CAudio(SOUND_TYPE _eType)
	: m_pChannel(NULL)
	, m_eType(_eType)
{
}

CAudio::~CAudio()
{
	if (NULL == m_pChannel)
		return;

	bool bplaying = false;
	m_pChannel->isPlaying(&bplaying);

	if (bplaying)
	{
		m_pChannel->stop();
	}
}

int CAudio::Update()
{
	if (SOUND_TYPE::TYPE_3D == m_eType || NULL != m_pChannel)
	{
		Vec3 vPos = Transform()->GetWorldPos();

		FMOD_VECTOR fPos = { vPos.x, vPos.y, vPos.z };

		m_pChannel->set3DAttributes(&fPos, NULL);
	}

	m_pChannel->setPaused(false);

	return 0;
}

void CAudio::Play(const wstring& _strKey, int _iRepeatCount)
{
	if (0 == _iRepeatCount)
		return;
	else if (_iRepeatCount < -1)
		assert(NULL);

	if (-1 != _iRepeatCount)
		_iRepeatCount -= 1;

	CSound* pSound = (CSound*)CResMgr::GetInst()->Load<CSound>(_strKey);

	if (NULL == pSound)
		assert(NULL);

	FMOD_MODE eMode;
	pSound->GetSound()->getMode(&eMode);

	if (NULL != m_pChannel)
		m_pChannel->stop();

	CSound::g_pSystem->playSound(pSound->GetSound(), NULL, true, &m_pChannel);
	m_pChannel->setMode(eMode);
	m_pChannel->setLoopCount(_iRepeatCount);
}

void CAudio::Pause()
{
	if (NULL == m_pChannel)
		return;

	m_pChannel->setPaused(true);
}
void CAudio::Stop()
{
	if (NULL == m_pChannel)
		return;

	m_pChannel->stop();
}

void CAudio::SetVolume(float _fVolume)
{
	if (NULL == m_pChannel)
		return;

	m_pChannel->setVolume(_fVolume);
}

void CAudio::Save(FILE * _pFile)
{
	//UINT iType = (UINT)COMPONENT_TYPE::AUDIO;
	//fwrite(&iType, sizeof(UINT), 1, _pFile);
	int a = 0;
}

void CAudio::Load(FILE * _pFile)
{
	int a = 0;
}
