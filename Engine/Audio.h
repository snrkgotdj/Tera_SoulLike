#pragma once
#include "Component.h"

#include "Sound.h"

enum class SOUND_TYPE
{
	TYPE_2D,
	TYPE_3D,
	END
};

class CAudio :
	public CComponent
{
private:
	FMOD::Channel*	m_pChannel;
	SOUND_TYPE		m_eType;

public:
	virtual int Update();

public:
	void Play(const wstring& _strKey, int _iRepeatCount = -1);
	void Pause();
	void Stop();
	void SetVolume(float _fVolume);

public:
	virtual void Save(FILE* _pFile);
	virtual void Load(FILE* _pFile);
	CLONE(CAudio);

public:
	CAudio();
	CAudio(SOUND_TYPE _eType);
	virtual ~CAudio();
};