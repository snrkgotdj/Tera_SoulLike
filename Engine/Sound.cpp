#include "Sound.h"

#include "SceneMgr.h"
#include "GameObject.h"
#include "Transform.h"

FMOD::System* CSound::g_pSystem = NULL;

CSound::CSound()
	: m_pSound(NULL)
{
}

CSound::~CSound()
{	
	if (NULL != m_pSound)
	{	
		m_pSound->release();
	}	
}

CSound * CSound::Create(const wstring & _strFullPath, bool _bIs3D)
{
	FMOD::Sound* pSound = NULL;
	
	string path(_strFullPath.begin(), _strFullPath.end());

	if (_bIs3D)
	{
		g_pSystem->createSound(path.c_str(), FMOD_3D | FMOD_3D_LINEARROLLOFF | FMOD_LOOP_NORMAL, NULL, &pSound);
		pSound->set3DMinMaxDistance(0.f, 3000.f);
	}
	else
	{
		g_pSystem->createSound(path.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, NULL, &pSound);
	}

	CSound* pNew = new CSound;
	pNew->m_pSound = pSound;

	return pNew;
}

int CSound::Update()
{
	Vec3 vPos = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Transform()->GetLocalPos();
	Vec3 vFront = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Transform()->GetDir(DIR_TYPE::FRONT);
	Vec3 vUp = CSceneMgr::GetInst()->GetCamera(CAMERA_TYPE::MAIN)->Transform()->GetDir(DIR_TYPE::UP);

	FMOD_VECTOR fPos = { vPos.x, vPos.y, vPos.z };
	FMOD_VECTOR fFront = { vFront.x, vFront.y, vFront.z };
	FMOD_VECTOR fUp = { vUp.x, vUp.y, vUp.z };

	CSound::g_pSystem->set3DListenerAttributes(0, &fPos, NULL, &fFront, &fUp);

	CSound::g_pSystem->update();

	return 0;
}