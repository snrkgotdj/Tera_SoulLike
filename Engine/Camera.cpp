#include "Camera.h"

#include "Transform.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "GameObject.h"

#include "InstancingBuffer.h"
#include "MeshRender.h"
#include "Animator_2D.h"
#include "Animator_3D.h"
#include "Collider.h"
#include "Particle.h"
#include "Trail.h"

#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Device.h"
#include "CamFilter.h"

#include "KeyMgr.h"
#include "ResMgr.h"

CCamera::CCamera()
	: m_Frustum(this)
	, m_pFilter(NULL)
	, m_pSkyBoxObj(NULL)
	, m_bPerspective(true)
	, m_fFOV(XM_PI / 4.f)
	, m_fWidth(0.f)
	, m_fHeight(0.f)
	, m_fScale(1.f)
	, m_fNear(1.f)
	, m_fFar(10000.f)
	, m_iLayerCheck(0)
	, m_eCamType(CAMERA_TYPE::END)
	, m_bLookAt(false)
	, m_bSkyBoxRender(true)
	, m_pPrevFiter(false)
{
	Vec2 vResolution = CRenderMgr::GetInst()->GetWindowResolution();
	m_fWidth = vResolution.x;
	m_fHeight = vResolution.y;

	m_Frustum.Init();
}

CCamera::~CCamera()
{
	Safe_Delete_Map(m_mapInstGroup_F);
	Safe_Delete_Map(m_mapInstGroup_D);
	Safe_Delete_Map(m_mapInstGroup_S);
	Safe_Delete_Map(m_mapInstGroup_P);
	SAFE_DELETE(m_pSkyBoxObj);
}

int CCamera::Update()
{

	return 0;
}

int CCamera::FinalUpdate()
{
	CreateViewMat(Transform());

	CreateProjMat();

	m_Frustum.FinalUpdate();

	if(nullptr != m_pHDRFilter)
		m_pHDRFilter->FinalUpdate();

	if (NULL != m_pFilter)
	{
		m_pFilter->FinalUpdate();
	}

	return 0;
}

void CCamera::Reset()
{
	Safe_Delete_Map(m_mapInstGroup_F);
	Safe_Delete_Map(m_mapInstGroup_D);
	Safe_Delete_Map(m_mapInstGroup_S);
	Safe_Delete_Map(m_mapInstGroup_P);
	m_mapInstGroup_F.clear();
	m_mapInstGroup_D.clear();
	m_mapInstGroup_S.clear();
	m_mapInstGroup_P.clear();
}

void CCamera::SetFilter(CCamFilter * _pFilter)
{
	if (m_pFilter == _pFilter)
		return;

	 m_pPrevFiter = m_pFilter; 
	 m_pFilter = _pFilter;
}

tRay CCamera::GetRay()
{
	D3D11_VIEWPORT tViewPort = {};
	UINT iViewPortCount = 1;
	CONTEXT->RSGetViewports(&iViewPortCount, &tViewPort);

	Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();
	tRay ray = {};
	ray.vOrigin = Transform()->GetWorldPos();

	ray.vDir.x = ((((vMousePos.x - tViewPort.TopLeftX) * 2.f / tViewPort.Width) - 1.f) - m_matProj._31) / m_matProj._11;
	ray.vDir.y = (-(((vMousePos.y - tViewPort.TopLeftY) * 2.f / tViewPort.Height) - 1.f) - m_matProj._32) / m_matProj._22;
	ray.vDir.z = 1.f;

	Matrix matInvView = XMMatrixInverse(NULL, m_matView);
	ray.vDir = XMVector3TransformNormal(ray.vDir.Convert(), matInvView);
	ray.vDir.Normalize();

	return ray;
}

Vec2 CCamera::Get2DWorldPosFromWindowPos(const Vec2& _vWindowPos, const Vec2& _vResolution)
{
	Vec2 vRatio = Vec2(WINSIZE_X / _vResolution.x, WINSIZE_Y / _vResolution.y);
	Vec2 vWindowPos = Vec2(_vWindowPos.x * vRatio.x, _vWindowPos.y * vRatio.y);
	Vec2 vWorldPos = Vec2(vWindowPos.x - (WINSIZE_X / 2.f), (WINSIZE_Y / 2.f) - vWindowPos.y);

	vWorldPos = Vec2(vWorldPos.x * m_fScale + Transform()->GetWorldPos().x
		, vWorldPos.y * m_fScale + Transform()->GetWorldPos().y);

	return vWorldPos;
}

void CCamera::UpdateInstancing()
{
	for (auto& pair : m_mapInstGroup_F)
		pair.second->clear();
	for (auto& pair : m_mapInstGroup_D)
		pair.second->clear();
	for (auto& pair : m_mapInstGroup_P)
		pair.second->clear();

	m_vecParticleObj.clear();
	m_vecColObj.clear();

	uInstID uID = {};

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

	for (UINT i = 0; i < vecLayer.size(); ++i)
	{
		if (NULL == vecLayer[i] || !(m_iLayerCheck & (1 << i)))
			continue;

		list<CGameObject*>& listObj = vecLayer[i]->GetObjList();
		for (auto & pObj : listObj)
		{
			//충돌 오브젝트면 추가
			if (pObj->Collider() != nullptr)
				m_vecColObj.push_back(pObj);

			// MeshRender 가 없으면 continue;
			if (pObj->MeshRender() == NULL || !pObj->IsActive())
				continue;

			// 카메라 시야에 들어 오는지 안오는지 체크
			Vec3 vWorldPos = pObj->Transform()->GetWorldPos();
			vWorldPos.y += pObj->Transform()->GetCulRadius();
			if (pObj->Transform()->IsCull() &&	false == m_Frustum.FrunstumCheckSphere(vWorldPos, pObj->Transform()->GetCulRadius()))
				continue;
			// 포스트 이펙트 오브젝트
			else if (pObj->MeshRender()->IsPostEffect())
			{
				// Gameobject 가 참조하는 Material 을 가져온다
				vector <CResPtr<CMaterial>>& vecMtrl = pObj->MeshRender()->GetMtrlVec();

				for (UINT iMtrl = 0; iMtrl < vecMtrl.size(); ++iMtrl)
				{
					if (NULL == vecMtrl[iMtrl] || vecMtrl[iMtrl]->GetShader() == NULL)
						continue;

					uID.llID = pObj->MeshRender()->GetInstID(iMtrl);
					map<ULONG64, vector<tInstObj>*>::iterator iter = m_mapInstGroup_P.find(uID.llID);
					if (iter == m_mapInstGroup_P.end())
					{
						vector<tInstObj>* pVec = new vector<tInstObj>;
						pVec->push_back(tInstObj{ pObj, iMtrl });
						m_mapInstGroup_P.insert(make_pair(uID.llID, pVec));
					}
					else
						iter->second->push_back(tInstObj{ pObj, iMtrl });
				}
			}
			else
			{
				// Gameobject 가 참조하는 Material 을 가져온다
				vector <CResPtr<CMaterial>>& vecMtrl = pObj->MeshRender()->GetMtrlVec();

				for (UINT iMtrl = 0; iMtrl < vecMtrl.size(); ++iMtrl)
				{
					if (NULL == vecMtrl[iMtrl] || vecMtrl[iMtrl]->GetShader() == NULL)
						continue;

					map<ULONG64, vector<tInstObj>*>* pMap = NULL;
					if (vecMtrl[iMtrl]->GetShader()->IsDeferred())
						pMap = &m_mapInstGroup_D;
					else
						pMap = &m_mapInstGroup_F;

					uID.llID = pObj->MeshRender()->GetInstID(iMtrl);
					map<ULONG64, vector<tInstObj>*>::iterator iter = pMap->find(uID.llID);
					if (iter == pMap->end())
					{
						vector<tInstObj>* pVec = new vector<tInstObj>;
						pVec->push_back(tInstObj{ pObj, iMtrl });
						pMap->insert(make_pair(uID.llID, pVec));
					}
					else
						iter->second->push_back(tInstObj{ pObj, iMtrl });
				}
			}
		}
	}
}

void CCamera::UpdateInstancing_shadow()
{
	for (auto& pair : m_mapInstGroup_S)
		pair.second->clear();

	uInstID uID = {};

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	const vector<CLayer*>& vecLayer = pCurScene->GetVecLayer();

	for (UINT i = 0; i < vecLayer.size(); ++i)
	{
		if (NULL == vecLayer[i] || !(m_iLayerCheck & (1 << i)))
			continue;

		list<CGameObject*>& listObj = vecLayer[i]->GetObjList();
		for (auto & pObj : listObj)
		{
			// MeshRender 가 없으면 continue;
			if (pObj->MeshRender() == NULL || pObj->MeshRender()->GetMesh() == nullptr || pObj->MeshRender()->CastShadow() == false || !pObj->IsActive())
				continue;

			// 카메라 시야에 들어 오는지 안오는지 체크	

			// Gameobject 가 참조하는 Mesh 의 ID 값을 키값으로 사용
			uID.sMesh = pObj->MeshRender()->GetMesh()->GetResID();

			map<ULONG64, vector<CGameObject*>*>::iterator iter = m_mapInstGroup_S.find(uID.llID);
			if (iter == m_mapInstGroup_S.end())
			{
				vector<CGameObject*>* pVec = new vector<CGameObject*>;
				pVec->push_back(pObj);
				m_mapInstGroup_S.insert(make_pair(uID.llID, pVec));
			}
			else
				iter->second->push_back(pObj);
		}
	}
}

void CCamera::CreateViewMat(const CTransform * _pTrans)
{
	m_matView = XMMatrixIdentity();

	if (m_bLookAt)
	{
		m_matView = m_matLookAt;
	}
	else
	{
		Vec3 vRight = _pTrans->GetDir(DIR_TYPE::RIGHT);
		Vec3 vUP = _pTrans->GetDir(DIR_TYPE::UP);
		Vec3 vFront = _pTrans->GetDir(DIR_TYPE::FRONT);
		Vec3 vPos = _pTrans->GetWorldPos();

		m_matView._11 = vRight.x;			m_matView._12 = vUP.x;			m_matView._13 = vFront.x;
		m_matView._21 = vRight.y;			m_matView._22 = vUP.y;			m_matView._23 = vFront.y;
		m_matView._31 = vRight.z;			m_matView._32 = vUP.z;			m_matView._33 = vFront.z;
		m_matView._41 = -vPos.Dot(vRight);  m_matView._42 = -vPos.Dot(vUP); m_matView._43 = -vPos.Dot(vFront);
	}
}

void CCamera::CreateProjMat()
{
	// 투영행렬 구하기
	if (m_bPerspective)
	{
		m_matProj = XMMatrixPerspectiveFovLH(m_fFOV, m_fWidth / m_fHeight, m_fNear, m_fFar);
	}
	else
	{
		if (CAMERA_TYPE::TOOL == m_eCamType)
		{
			m_fWidth = WINSIZE_X;
			m_fHeight = WINSIZE_Y;
		}

		m_matProj = XMMatrixOrthographicLH(m_fWidth * m_fScale, m_fHeight * m_fScale, 1.f, 1000000.f);
		//m_matProj = XMMatrixOrthographicOffCenterLH(-m_fWidth * m_fScale / 2.f, m_fWidth * m_fScale / 2.f, -m_fHeight * m_fScale / 2.f, m_fHeight * m_fScale / 2.f, m_fNear, m_fFar);
	}
}


void CCamera::RenderFilter()
{
	if (NULL == m_pFilter)
		assert(NULL);

	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
	m_pFilter->Render();
}

void CCamera::RenderToolObj()
{
	// 툴용 오브젝트 랜더링
	for (auto& pObj : m_vecToolObj)
	{
		pObj->Render();
	}
}

void CCamera::RenderCollider()
{
	for (auto& pCol : m_vecColObj)
	{
		if (false == pCol->IsActive())
			continue;

		pCol->Collider()->Render();
	}
}

void CCamera::RenderSkyBox()
{
	if (NULL != m_pSkyBoxObj && m_bSkyBoxRender == true)
		m_pSkyBoxObj->Render();
}

void CCamera::Render_ShadowMap(CMaterial * _pMtrl)
{
	m_vecSingleObj_S.clear();
	tInstancingData tInstData = {};
	for (auto& pair : m_mapInstGroup_S)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second->empty())
			continue;
		else if (pair.second->size() == 1)
		{
			m_vecSingleObj_S.push_back(pair.second->at(0));
			continue;
		}

		CGameObject* pObj = pair.second->front();
		CMesh* pMesh = pObj->MeshRender()->GetMesh();

		if (NULL == _pMtrl->GetShader())
			continue;

		CInstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;
		for (UINT i = 0; i < pair.second->size(); ++i)
		{
			if (pair.second->at(i)->Animator_2D())
			{
				m_vecSingleObj_S.push_back(pair.second->at(i));
				continue;
			}

			tInstData.matWorld = pair.second->at(i)->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;	 

			if (pair.second->at(i)->Animator_3D() && pair.second->at(i)->Animator_3D()->IsPlay())
			{
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second->at(i)->Animator_3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			tInstData.iBoneCount = CInstancingBuffer::GetInst()->GetBoneCount();
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		CInstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());

		_pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(_pMtrl->GetShader()->GetVSInstBlob());
		pMesh->UpdateDataInstancing();

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		CAnimator_2D::Disable();

		UINT iSubCount = pMesh->GetSubmeshCount();
		for (UINT i = 0; i < iSubCount; ++i)
		{
			pMesh->renderInstancing(i);
		}
	}

	// 개별 랜더링
	for (auto& pObj : m_vecSingleObj_S)
	{
		_pMtrl->UpdateData();
		pObj->Transform()->UpdateData();
		if (NULL != pObj->Animator_3D())
			pObj->Animator_3D()->UpdateData();
		
		CMesh* pMesh = pObj->MeshRender()->GetMesh();
		pMesh->SetLayout(_pMtrl->GetShader()->GetVSInstBlob());
		if (pObj->Terrain())
			pMesh->UpdateData(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		else
			pMesh->UpdateData();

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);

		UINT iSubCount = pMesh->GetSubmeshCount();
		for (UINT i = 0; i < iSubCount; ++i)
		{
			pMesh->Render(i);
		}
	}

}

bool ZOrder(const pair<INT_PTR, vector<tInstObj>>& _pLeft, const pair<INT_PTR, vector<tInstObj>>& _pRight)
{
	return _pLeft.second.front().pObj->Transform()->GetLocalPos().z > _pRight.second.front().pObj->Transform()->GetLocalPos().z;
}

void CCamera::RenderForward(bool _bIsUI)
{
	g_Transform.matProj = XMMatrixTranspose(m_matProj);
	g_Transform.matView = XMMatrixTranspose(m_matView);
	g_Transform.matInvView = XMMatrixInverse(NULL, g_Transform.matView);
	g_Transform.matInvProj = XMMatrixInverse(NULL, g_Transform.matProj);

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_F)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second->empty())
			continue;
		else if (pair.second->size() == 1)
		{
			map<INT_PTR, vector<tInstObj>>::iterator iter
				= m_mapSingleObj.find((INT_PTR)pair.second->at(0).pObj);

			if (iter != m_mapSingleObj.end())
				iter->second.push_back(pair.second->at(0));
			else
			{
				m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(0).pObj, vector<tInstObj>{pair.second->at(0)}));
			}
			continue;
		}

		CGameObject* pObj = pair.second->front().pObj;
		CMesh* pMesh = pObj->MeshRender()->GetMesh();
		CMaterial* pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second->front().iMtrlIdx);

		if (NULL == pMtrl->GetShader())
			continue;

		CInstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;

		for (UINT i = 0; i < pair.second->size(); ++i)
		{
			if (pair.second->at(i).pObj->Animator_2D())
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second->at(i).pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second->at(i));
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(i).pObj, vector<tInstObj>{pair.second->at(i)}));
				}
				continue;
			}

			tInstData.matWorld = pair.second->at(i).pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second->at(i).pObj->Animator_3D() 
				&& (pair.second->at(i).pObj->Animator_3D()->IsPlay() || pair.second->at(i).pObj->Animator_3D()->IsStop()))
			{
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second->at(i).pObj->Animator_3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			tInstData.iBoneCount = CInstancingBuffer::GetInst()->GetBoneCount();
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		CInstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());

		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader()->GetVSInstBlob());
		pMesh->UpdateDataInstancing();

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		CAnimator_2D::Disable();
		pMesh->renderInstancing(pair.second->front().iMtrlIdx);
	}


	if (_bIsUI)
	{
		vector<pair<INT_PTR, vector<tInstObj>>> vecTemp(m_mapSingleObj.begin(), m_mapSingleObj.end());
		sort(vecTemp.begin(), vecTemp.end(), ZOrder);

		// 개별 랜더링
		for (auto& pair : vecTemp)
		{
			if (pair.second.empty())
				continue;

			pair.second[0].pObj->Transform()->UpdateData();

			if (pair.second[0].pObj->Animator_3D())
				pair.second[0].pObj->Animator_3D()->UpdateData();
			else if (pair.second[0].pObj->Animator_2D())
				pair.second[0].pObj->Animator_2D()->UpdateData();

			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				if (NULL != pair.second[i].pObj->Trail())
				{
					pair.second[i].pObj->Trail()->Render();
					if (false == CSceneMgr::GetInst()->IsPlayMode())
					{
						pair.second[i].pObj->Trail()->ToolRender();
						pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
					}
					continue;
				}

				pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
			}

			if (pair.second[0].pObj->Animator_3D())
				pair.second[0].pObj->Animator_3D()->ClearData();
			else if (pair.second[0].pObj->Animator_2D())
				pair.second[0].pObj->Animator_2D()->Disable();
		}
	}
	else
	{
		// 개별 랜더링
		for (auto& pair : m_mapSingleObj)
		{
			if (pair.second.empty())
				continue;

			pair.second[0].pObj->Transform()->UpdateData();

			if (pair.second[0].pObj->Animator_3D())
				pair.second[0].pObj->Animator_3D()->UpdateData();
			else if (pair.second[0].pObj->Animator_2D())
				pair.second[0].pObj->Animator_2D()->UpdateData();

			for (UINT i = 0; i < pair.second.size(); ++i)
			{
				if (NULL != pair.second[i].pObj->Trail())
				{
					pair.second[i].pObj->Trail()->Render();
					if (false == CSceneMgr::GetInst()->IsPlayMode())
					{
						pair.second[i].pObj->Trail()->ToolRender();
						pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
					}
					continue;
				}

				pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
			}

			if (pair.second[0].pObj->Animator_3D())
				pair.second[0].pObj->Animator_3D()->ClearData();
			else if (pair.second[0].pObj->Animator_2D())
				pair.second[0].pObj->Animator_2D()->Disable();

		}
	}


	// 파티클 렌더링
	for (auto& pParticleObj : m_vecParticleObj)
	{
		pParticleObj->Particle()->Render();
	}
}

void CCamera::RenderDeferred()
{
	g_Transform.matView = XMMatrixTranspose(m_matView);
	g_Transform.matProj = XMMatrixTranspose(m_matProj);
	g_Transform.matInvView = XMMatrixInverse(nullptr, g_Transform.matView);
	g_Transform.matInvProj = XMMatrixInverse(nullptr, g_Transform.matProj);

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_D)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second->empty())
			continue;
		else if (pair.second->size() == 1)
		{
			map<INT_PTR, vector<tInstObj>>::iterator iter
				= m_mapSingleObj.find((INT_PTR)pair.second->at(0).pObj);

			if (iter != m_mapSingleObj.end())
				iter->second.push_back(pair.second->at(0));
			else
			{
				m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(0).pObj, vector<tInstObj>{pair.second->at(0)}));
			}
			continue;
		}

		CGameObject* pObj = pair.second->front().pObj;
		CMesh* pMesh = pObj->MeshRender()->GetMesh();
		CMaterial* pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second->front().iMtrlIdx);

		if (NULL == pMtrl->GetShader())
			continue;

		CInstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;
		for (UINT i = 0; i < pair.second->size(); ++i)
		{
			if (pair.second->at(i).pObj->Animator_2D() /*|| pair.second->at(i).pObj->Animator_3D()*/)
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second->at(i).pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second->at(i));
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(i).pObj, vector<tInstObj>{pair.second->at(i)}));
				}
				continue;
			}

			tInstData.matWorld = pair.second->at(i).pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second->at(i).pObj->Animator_3D() 
				&& (pair.second->at(i).pObj->Animator_3D()->IsPlay() || pair.second->at(i).pObj->Animator_3D()->IsStop()))
			{
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second->at(i).pObj->Animator_3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			tInstData.iBoneCount = CInstancingBuffer::GetInst()->GetBoneCount();
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);
		}

		CInstancingBuffer::GetInst()->SetData(pObj->MeshRender()->GetMesh()->GetBoneTex());

		pObj->Transform()->UpdateData();
		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader()->GetVSInstBlob());
		pMesh->UpdateDataInstancing();

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		CAnimator_2D::Disable();
		pMesh->renderInstancing(pair.second->front().iMtrlIdx);
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		if (pair.second[0].pObj->Animator_3D())
			pair.second[0].pObj->Animator_3D()->UpdateData();
		else if (pair.second[0].pObj->Animator_2D())
			pair.second[0].pObj->Animator_2D()->UpdateData();

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator_3D())
			pair.second[0].pObj->Animator_3D()->ClearData();
		else if (pair.second[0].pObj->Animator_2D())
			pair.second[0].pObj->Animator_2D()->Disable();
	}
}

void CCamera::RenderPostEffect()
{
	g_Transform.matView = XMMatrixTranspose(m_matView);
	g_Transform.matProj = XMMatrixTranspose(m_matProj);
	g_Transform.matInvView = XMMatrixInverse(nullptr, g_Transform.matView);
	g_Transform.matInvProj = XMMatrixInverse(nullptr, g_Transform.matProj);

	for (auto& pair : m_mapSingleObj)
	{
		pair.second.clear();
	}

	tInstancingData tInstData = {};

	for (auto& pair : m_mapInstGroup_P)
	{
		// 그룹 오브젝트가 없거나, 쉐이더가 없는 경우
		if (pair.second->empty())
			continue;
		else if (pair.second->size() == 1)
		{
			map<INT_PTR, vector<tInstObj>>::iterator iter
				= m_mapSingleObj.find((INT_PTR)pair.second->at(0).pObj);

			if (iter != m_mapSingleObj.end())
				iter->second.push_back(pair.second->at(0));
			else
			{
				m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(0).pObj, vector<tInstObj>{pair.second->at(0)}));
			}
			continue;
		}

		CGameObject* pObj = pair.second->front().pObj;
		CMesh* pMesh = pObj->MeshRender()->GetMesh();
		CMaterial* pMtrl = pObj->MeshRender()->GetSharedMaterial(pair.second->front().iMtrlIdx);

		if (NULL == pMtrl->GetShader())
			continue;

		CInstancingBuffer::GetInst()->Clear();
		int iRowIdx = 0;
		for (UINT i = 0; i < pair.second->size(); ++i)
		{
			if (pair.second->at(i).pObj->Animator_2D() || pair.second->at(i).pObj->Trail() || pair.second->at(i).pObj->Animator_3D())
			{
				map<INT_PTR, vector<tInstObj>>::iterator iter
					= m_mapSingleObj.find((INT_PTR)pair.second->at(i).pObj);

				if (iter != m_mapSingleObj.end())
					iter->second.push_back(pair.second->at(i));
				else
				{
					m_mapSingleObj.insert(make_pair((INT_PTR)pair.second->at(i).pObj, vector<tInstObj>{pair.second->at(i)}));
				}
				continue;
			}

			tInstData.matWorld = pair.second->at(i).pObj->Transform()->GetWorldMat();
			tInstData.matWV = tInstData.matWorld * m_matView;
			tInstData.matWVP = tInstData.matWV * m_matProj;

			if (pair.second->at(i).pObj->Animator_3D()
				&& (pair.second->at(i).pObj->Animator_3D()->IsPlay() || pair.second->at(i).pObj->Animator_3D()->IsStop()))
			{
				tInstData.iRowIdx = iRowIdx++;
				CInstancingBuffer::GetInst()->AddInstancingBoneMat(pair.second->at(i).pObj->Animator_3D()->GetFinalBoneMat());
			}
			else
				tInstData.iRowIdx = -1;

			tInstData.iBoneCount = CInstancingBuffer::GetInst()->GetBoneCount();
			CInstancingBuffer::GetInst()->AddInstancingData(tInstData);

		}

		if (pair.second->front().pObj->Animator_2D() || pair.second->front().pObj->Trail())
			continue;

		CInstancingBuffer::GetInst()->SetData(NULL);

		pMtrl->UpdateDataInstancing();
		pMesh->SetLayout(pMtrl->GetShader()->GetVSInstBlob());
		pMesh->UpdateDataInstancing();

		CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
		CAnimator_2D::Disable();
		pMesh->renderInstancing(pair.second->front().iMtrlIdx);
	}

	// 개별 랜더링
	for (auto& pair : m_mapSingleObj)
	{
		if (pair.second.empty())
			continue;

		pair.second[0].pObj->Transform()->UpdateData();

		if (pair.second[0].pObj->Animator_3D())
			pair.second[0].pObj->Animator_3D()->UpdateData();
		else if (pair.second[0].pObj->Animator_2D())
			pair.second[0].pObj->Animator_2D()->UpdateData();

		for (UINT i = 0; i < pair.second.size(); ++i)
		{
			if (NULL != pair.second[i].pObj->Trail())
			{
				pair.second[i].pObj->Trail()->Render();
				if (false == CSceneMgr::GetInst()->IsPlayMode())
				{
					pair.second[i].pObj->Trail()->ToolRender();
					//pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
				}
				continue;
			}

			pair.second[i].pObj->MeshRender()->Render(pair.second[i].iMtrlIdx);
		}

		if (pair.second[0].pObj->Animator_3D())
			pair.second[0].pObj->Animator_3D()->ClearData();
		else if (pair.second[0].pObj->Animator_2D())
			pair.second[0].pObj->Animator_2D()->Disable();
	}
}

bool CCamera::RenderHDR()
{
	if (NULL == m_pHDRFilter)
		return false;

	CDevice::GetInst()->SetRasterizer(RASTERIZE_TYPE::RT_BACK);
	m_pHDRFilter->Render();

	return true;
}

void CCamera::LayerRenderCheck(const wstring& _strLayerName)
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CLayer* pLayer = pCurScene->FindLayer(_strLayerName);

	if (NULL == pLayer)
		assert(NULL);

	int iIdx = pLayer->GetLayerIdx();

	UINT iCompare = 1 << iIdx;

	if (m_iLayerCheck & iCompare)
	{
		m_iLayerCheck &= ~iCompare;
	}
	else
	{
		m_iLayerCheck |= iCompare;
	}
} 

void CCamera::CreateSkyBox()
{
	// 스카이 박스 하나 만들기	
	CGameObject* pSkyBox = new CGameObject;
	pSkyBox->SetTag(L"SkyBox");
	pSkyBox->AddComponent<CTransform>(new CTransform);
	pSkyBox->AddComponent<CMeshRender>(new CMeshRender);

	pSkyBox->Transform()->SetLocalPos(Vec3(0.f, 0.f, 0.f));
	pSkyBox->Transform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
	pSkyBox->Transform()->SetCamCull(false);

	CMaterial* pMtrl = CResMgr::GetInst()->FindMaterial(L"Mtrl_SkyCube");

	pSkyBox->MeshRender()->SetMesh((CMesh*)CResMgr::GetInst()->Load<CMesh>(L"SphereMesh"));
	pSkyBox->MeshRender()->SetMaterial(pMtrl);
	pSkyBox->MeshRender()->SetRSMode(RASTERIZE_TYPE::RT_FRONT);

	CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Skybox_cube", L"Texture\\Skybox\\cubemap_02.dds");
	pMtrl->SetData(SHADER_PARAM::SP_TEXARR_7, &pTex);
	//CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(L"Skybox", L"Texture\\Skybox\\Skybox02.png");
	//pMtrl->SetData(SHADER_PARAM::SP_TEX_0, &pTex);
	m_pSkyBoxObj = pSkyBox;
}

CGameObject * CCamera::GetObjectByPicking(CLayer* _pLayer)
{
	// Ray를 얻어온다
	tRay ray = GetRay();

	return 	_pLayer->FindObjectByRay(ray);
}

void CCamera::Save(FILE * _pFile)
{
	UINT iType = (UINT)COMPONENT_TYPE::CAMERA;
	fwrite(&iType, sizeof(UINT), 1, _pFile);

	fwrite(&m_bPerspective, 1, 1, _pFile);
	fwrite(&m_fFOV, sizeof(float), 1, _pFile);
	fwrite(&m_fWidth, sizeof(float), 1, _pFile);
	fwrite(&m_fHeight, sizeof(float), 1, _pFile);
	fwrite(&m_fScale, sizeof(float), 1, _pFile);
	fwrite(&m_fNear, sizeof(float), 1, _pFile);
	fwrite(&m_fFar, sizeof(float), 1, _pFile);
	fwrite(&m_iLayerCheck, sizeof(UINT), 1, _pFile);

	bool bIsHaveSkyBox = false;
	if (NULL != m_pSkyBoxObj)
		bIsHaveSkyBox = true;

	fwrite(&bIsHaveSkyBox, sizeof(bool), 1, _pFile);

	fwrite(&m_eCamType, sizeof(CAMERA_TYPE), 1, _pFile);
}

void CCamera::Load(FILE * _pFile)
{
	fread(&m_bPerspective, 1, 1, _pFile);
	fread(&m_fFOV, sizeof(float), 1, _pFile);
	fread(&m_fWidth, sizeof(float), 1, _pFile);
	fread(&m_fHeight, sizeof(float), 1, _pFile);
	fread(&m_fScale, sizeof(float), 1, _pFile);
	fread(&m_fNear, sizeof(float), 1, _pFile);
	fread(&m_fFar, sizeof(float), 1, _pFile);
	fread(&m_iLayerCheck, sizeof(UINT), 1, _pFile);

	bool bIsHaveSkyBox = false;
	fread(&bIsHaveSkyBox, sizeof(bool), 1, _pFile);

	if (bIsHaveSkyBox)
		CreateSkyBox();

	fread(&m_eCamType, sizeof(CAMERA_TYPE), 1, _pFile);
	CSceneMgr::GetInst()->SetCamera(m_eCamType, GetGameObject());
}