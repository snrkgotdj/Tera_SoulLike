#include "FBXLoader.h"
#include "mesh.h"

#include "ResMgr.h"
#include "PathMgr.h"

#include "Material.h"
#include "ShaderMgr.h"

CFBXLoader::CFBXLoader()
	: m_pManager(NULL)
	, m_pScene(NULL)
	, m_pImporter(NULL)
{
}

CFBXLoader::~CFBXLoader()
{
	m_pScene->Destroy();
	m_pManager->Destroy();

	for (UINT i = 0; i < m_vecBone.size(); ++i)
	{
		SAFE_DELETE(m_vecBone[i]);
	}

	for (UINT i = 0; i < m_vecAnimClip.size(); ++i)
	{
		SAFE_DELETE(m_vecAnimClip[i]);
	}
}

void CFBXLoader::init()
{
	m_pManager = FbxManager::Create();
	if (NULL == m_pManager)
		assert(NULL);

	FbxIOSettings* pIOSettings = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(pIOSettings);

	m_pScene = FbxScene::Create(m_pManager, "");
	if (NULL == m_pScene)
		assert(NULL);
}

void CFBXLoader::LoadFbx(const wstring & _strPath)
{
	m_vecContainer.clear();

	m_pImporter = FbxImporter::Create(m_pManager, "");

	//wstring str = wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(strName.c_str());
	string strPath(_strPath.begin(), _strPath.end());

	m_pImporter->Initialize(strPath.c_str(), -1, m_pManager->GetIOSettings());
	m_pImporter->Import(m_pScene);

	/*FbxAxisSystem originAxis = FbxAxisSystem::eMax;
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem DesireAxis = FbxAxisSystem::DirectX;
	DesireAxis.ConvertScene(m_pScene);
	originAxis = m_pScene->GetGlobalSettings().GetAxisSystem();*/

	m_pScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::Max);

	// Bone 정보 읽기
	LoadSkeleton(m_pScene->GetRootNode());

	// Animation 이름정보 
	m_pScene->FillAnimStackNameArray(m_arrAnimName);

	// Animation Clip 정보
	LoadAnimationClip();

	// 삼각화(Triangulate)
	Triangulate(m_pScene->GetRootNode());

	// 메쉬 데이터 얻기
	LoadMeshDataFromNode(m_pScene->GetRootNode());
	m_pImporter->Destroy();

	// 필요한 텍스쳐 로드
	LoadTexture();

	// 필요한 메테리얼 생성
	CreateMaterial();
}

void CFBXLoader::LoadMeshDataFromNode(FbxNode * _pNode)
{
	// 노드의 메쉬정보 읽기
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

	if (pAttr && FbxNodeAttribute::eMesh == pAttr->GetAttributeType())
	{
		FbxMesh* pMesh = _pNode->GetMesh();
		if (NULL != pMesh)
			LoadMesh(pMesh);
	}

	// 해당 노드의 재질정보 읽기
	UINT iMtrlCnt = _pNode->GetMaterialCount();
	if (iMtrlCnt > 0)
	{
		for (UINT i = 0; i < iMtrlCnt; ++i)
		{
			FbxSurfaceMaterial* pMtrlSur = _pNode->GetMaterial(i);
			LoadMaterial(pMtrlSur);
		}
	}

	// 자식 노드 정보 읽기
	int iChildCnt = _pNode->GetChildCount();
	for (int i = 0; i < iChildCnt; ++i)
	{
		LoadMeshDataFromNode(_pNode->GetChild(i));
	}
}

void CFBXLoader::LoadMesh(FbxMesh * _pFbxMesh)
{
	m_vecContainer.push_back(tContainer{});
	tContainer& Container = m_vecContainer[m_vecContainer.size() - 1];

	string strName = _pFbxMesh->GetName();
	Container.strName = wstring(strName.begin(), strName.end());

	int iVtxCnt = _pFbxMesh->GetControlPointsCount();
	Container.Resize(iVtxCnt);

	FbxVector4* pFbxPos = _pFbxMesh->GetControlPoints();

	for (int i = 0; i < iVtxCnt; ++i)
	{
		Container.vecPos[i].x = (float)pFbxPos[i].mData[0];
		Container.vecPos[i].y = (float)pFbxPos[i].mData[2];
		Container.vecPos[i].z = (float)pFbxPos[i].mData[1];
	}

	// 폴리곤 개수
	int iPolyCnt = _pFbxMesh->GetPolygonCount();

	// 재질의 개수 ( ==> SubSet 개수 ==> Index Buffer Count)
	int iMtrlCnt = _pFbxMesh->GetNode()->GetMaterialCount();
	Container.vecIdx.resize(iMtrlCnt);

	// 정점 정보가 속한 subset 을 알기위해서...
	FbxGeometryElementMaterial* pMtrl = _pFbxMesh->GetElementMaterial();

	// 폴리곤을 구성하는 정점 개수
	int iPolySize = _pFbxMesh->GetPolygonSize(0);
	if (3 != iPolySize)
		assert(NULL); // Polygon 구성 정점이 3개가 아닌 경우

	UINT arrIdx[3] = {};
	UINT iVtxOrder = 0; // 폴리곤 순서로 접근하는 순번

	for (int i = 0; i < iPolyCnt; ++i)
	{
		for (int j = 0; j < iPolySize; ++j)
		{
			// i 번째 폴리곤에, j 번째 정점
			int iIdx = _pFbxMesh->GetPolygonVertex(i, j);
			arrIdx[j] = iIdx;

			GetTangent(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetBinormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetNormal(_pFbxMesh, &Container, iIdx, iVtxOrder);
			GetUV(_pFbxMesh, &Container, iIdx, _pFbxMesh->GetTextureUVIndex(i, j));

			++iVtxOrder;
		}
		UINT iSubsetIdx = pMtrl->GetIndexArray().GetAt(i);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[0]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[2]);
		Container.vecIdx[iSubsetIdx].push_back(arrIdx[1]);
	}

	LoadAnimationData(_pFbxMesh, &Container);
}

void CFBXLoader::LoadMaterial(FbxSurfaceMaterial * _pMtrlSur)
{
	tFbxMaterial tMtrlInfo{};

	string str = _pMtrlSur->GetName();
	tMtrlInfo.strMtrlName = wstring(str.begin(), str.end());

	// Diff
	tMtrlInfo.tMtrl.vMtrlDiff = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sDiffuse
		, FbxSurfaceMaterial::sDiffuseFactor);

	// Amb
	tMtrlInfo.tMtrl.vMtrlAmb = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sAmbient
		, FbxSurfaceMaterial::sAmbientFactor);

	// Spec
	tMtrlInfo.tMtrl.vMtrlSpec = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sSpecular
		, FbxSurfaceMaterial::sSpecularFactor);

	// Emisv
	tMtrlInfo.tMtrl.vMtrlEmiv = GetMtrlData(_pMtrlSur
		, FbxSurfaceMaterial::sEmissive
		, FbxSurfaceMaterial::sEmissiveFactor);

	// Texture Name
	tMtrlInfo.strDiff = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sDiffuse);

	// 노말 텍스쳐가 없다면 범프맵을 찾는다
	tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sNormalMap);
	if(tMtrlInfo.strNormal.empty())
		tMtrlInfo.strNormal = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sBump);

	tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sSpecular);
	if (tMtrlInfo.strSpec.empty())
		tMtrlInfo.strSpec = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sReflection);

	tMtrlInfo.strEmis = GetMtrlTextureName(_pMtrlSur, FbxSurfaceMaterial::sEmissive);

	m_vecContainer.back().vecMtrl.push_back(tMtrlInfo);
}

void CFBXLoader::GetTangent(FbxMesh * _pMesh
	, tContainer * _pContainer
	, int _iIdx		 /*해당 정점의 인덱스*/
	, int _iVtxOrder /*폴리곤 단위로 접근하는 순서*/)
{
	int iTangentCnt = _pMesh->GetElementTangentCount();
	if (1 != iTangentCnt)
		assert(NULL); // 정점 1개가 포함하는 탄젠트 정보가 2개 이상이다.

					  // 탄젠트 data 의 시작 주소
	FbxGeometryElementTangent* pTangent = _pMesh->GetElementTangent();
	UINT iTangentIdx = 0;

	if (pTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = _iVtxOrder;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pTangent->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pTangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			iTangentIdx = _iIdx;
		else
			iTangentIdx = pTangent->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vTangent = pTangent->GetDirectArray().GetAt(iTangentIdx);

	_pContainer->vecTangent[_iIdx].x = (float)vTangent.mData[0];
	_pContainer->vecTangent[_iIdx].y = (float)vTangent.mData[2];
	_pContainer->vecTangent[_iIdx].z = (float)vTangent.mData[1];
}

void CFBXLoader::GetBinormal(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iVtxOrder)
{
	int iBinormalCnt = _pMesh->GetElementBinormalCount();
	if (1 != iBinormalCnt)
		assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.

					  // 종법선 data 의 시작 주소
	FbxGeometryElementBinormal* pBinormal = _pMesh->GetElementBinormal();
	UINT iBinormalIdx = 0;

	if (pBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = _iVtxOrder;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pBinormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pBinormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iBinormalIdx = _iIdx;
		else
			iBinormalIdx = pBinormal->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIdx);

	_pContainer->vecBinormal[_iIdx].x = (float)vBinormal.mData[0];
	_pContainer->vecBinormal[_iIdx].y = (float)vBinormal.mData[2];
	_pContainer->vecBinormal[_iIdx].z = (float)vBinormal.mData[1];
}

void CFBXLoader::GetNormal(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iVtxOrder)
{
	int iNormalCnt = _pMesh->GetElementNormalCount();
	if (1 != iNormalCnt)
		assert(NULL); // 정점 1개가 포함하는 종법선 정보가 2개 이상이다.

					  // 종법선 data 의 시작 주소
	FbxGeometryElementNormal* pNormal = _pMesh->GetElementNormal();
	UINT iNormalIdx = 0;

	if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iVtxOrder;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iVtxOrder);
	}
	else if (pNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (pNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
			iNormalIdx = _iIdx;
		else
			iNormalIdx = pNormal->GetIndexArray().GetAt(_iIdx);
	}

	FbxVector4 vNormal = pNormal->GetDirectArray().GetAt(iNormalIdx);

	_pContainer->vecNormal[_iIdx].x = (float)vNormal.mData[0];
	_pContainer->vecNormal[_iIdx].y = (float)vNormal.mData[2];
	_pContainer->vecNormal[_iIdx].z = (float)vNormal.mData[1];
}

void CFBXLoader::GetUV(FbxMesh * _pMesh, tContainer * _pContainer, int _iIdx, int _iUVIndex)
{
	FbxGeometryElementUV* pUV = _pMesh->GetElementUV();

	UINT iUVIdx = 0;
	if (pUV->GetReferenceMode() == FbxGeometryElement::eDirect)
		iUVIdx = _iIdx;
	else
		iUVIdx = pUV->GetIndexArray().GetAt(_iIdx);

	iUVIdx = _iUVIndex;
	FbxVector2 vUV = pUV->GetDirectArray().GetAt(iUVIdx);
	_pContainer->vecUV[_iIdx].x = (float)vUV.mData[0];
	_pContainer->vecUV[_iIdx].y = 1.f - (float)vUV.mData[1]; // fbx uv 좌표계는 좌하단이 0,0
}

Vec4 CFBXLoader::GetMtrlData(FbxSurfaceMaterial * _pSurface
	, const char * _pMtrlName
	, const char * _pMtrlFactorName)
{
	FbxDouble3  vMtrl;
	FbxDouble	dFactor = 0.f;

	FbxProperty tMtrlProperty = _pSurface->FindProperty(_pMtrlName);
	FbxProperty tMtrlFactorProperty = _pSurface->FindProperty(_pMtrlFactorName);

	if (tMtrlProperty.IsValid() && tMtrlFactorProperty.IsValid())
	{
		vMtrl = tMtrlProperty.Get<FbxDouble3>();
		dFactor = tMtrlFactorProperty.Get<FbxDouble>();
	}

	Vec4 vRetVal = Vec4((float)(vMtrl.mData[0] * dFactor), (float)(vMtrl.mData[1] * dFactor), (float)(vMtrl.mData[2] * dFactor), (float)dFactor);
	return vRetVal;
}

wstring CFBXLoader::GetMtrlTextureName(FbxSurfaceMaterial * _pSurface, const char * _pMtrlProperty)
{
	string strName;

	FbxProperty TextureProperty = _pSurface->FindProperty(_pMtrlProperty);
	if (TextureProperty.IsValid())
	{
		UINT iCnt = TextureProperty.GetSrcObjectCount();

		if (1 <= iCnt)
		{
			FbxFileTexture* pFbxTex = TextureProperty.GetSrcObject<FbxFileTexture>(0);
			if (NULL != pFbxTex)
				strName = pFbxTex->GetFileName();
		}
	}

	return wstring(strName.begin(), strName.end());
}

void CFBXLoader::LoadTexture()
{
	wstring strResPath = CPathMgr::GetResPath();

	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			wstring strPath;
			wstring strFileName;

			strPath = CPathMgr::GetRelativePath(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			strFileName = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			CResMgr::GetInst()->Load<CTexture>(strFileName, strPath);

			strPath = CPathMgr::GetRelativePath(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			strFileName = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			CResMgr::GetInst()->Load<CTexture>(strFileName, strPath);

			strPath = CPathMgr::GetRelativePath(m_vecContainer[i].vecMtrl[j].strSpec.c_str());
			strFileName = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strSpec.c_str());
			CResMgr::GetInst()->Load<CTexture>(strFileName, strPath);

			if (m_vecContainer[i].vecMtrl[j].strEmis.empty())
				continue;

			strPath = CPathMgr::GetRelativePath(m_vecContainer[i].vecMtrl[j].strEmis.c_str());
			strFileName = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strEmis.c_str());
			CResMgr::GetInst()->Load<CTexture>(strFileName, strPath);
		}
	}
}

void CFBXLoader::CreateMaterial()
{
	wstring strKey;
	for (UINT i = 0; i < m_vecContainer.size(); ++i)
	{
		for (UINT j = 0; j < m_vecContainer[i].vecMtrl.size(); ++j)
		{
			CMaterial* pMaterial = new CMaterial;
			pMaterial->SetShader(CShaderMgr::GetInst()->FindShader(L"Shader_StdDeferred"));

			wstring strKey = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			CTexture* pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::SP_TEX_0, &pTex);

			strKey = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strNormal.c_str());
			pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::SP_TEX_1, &pTex);

			strKey = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strSpec.c_str());
			pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::SP_TEX_2, &pTex);

			strKey = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strEmis.c_str());
			pTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(strKey);
			if (NULL != pTex)
				pMaterial->SetData(SHADER_PARAM::SP_TEX_3, &pTex);

			strKey = CPathMgr::GetFileName(m_vecContainer[i].vecMtrl[j].strDiff.c_str());
			strKey += L"_";
			strKey += to_wstring(i);
			CResMgr::GetInst()->AddMtrl(m_vecContainer[i].vecMtrl[j].strMtrlName, pMaterial);
		}
	}
}

void CFBXLoader::LoadSkeleton(FbxNode * _pNode)
{
	int iChildCount = _pNode->GetChildCount();

	LoadSkeleton_Re(_pNode, 0, 0, -1);
}

void CFBXLoader::LoadSkeleton_Re(FbxNode * _pNode, int _iDepth, int _iIdx, int _iParentIdx)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		tBone* pBone = new tBone;

		string strBoneName = _pNode->GetName();

		pBone->strBoneName = wstring(strBoneName.begin(), strBoneName.end());
		pBone->iDepth = _iDepth++;
		pBone->iParentIdx = _iParentIdx;

		m_vecBone.push_back(pBone);
	}

	int iChildCount = _pNode->GetChildCount();
	for (int i = 0; i < iChildCount; ++i)
	{
		LoadSkeleton_Re(_pNode->GetChild(i), _iDepth, m_vecBone.size(), _iIdx);
	}
}

void CFBXLoader::LoadAnimationClip()
{
	int iAnimCount = m_arrAnimName.GetCount();

	for (int i = 0; i < iAnimCount; ++i)
	{
		FbxAnimStack* pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_arrAnimName[i]->Buffer());

		if (!pAnimStack)
			continue;

		tAnimClip* pAnimClip = new tAnimClip;

		string strClipName = pAnimStack->GetName();
		pAnimClip->strClipName = wstring(strClipName.begin(), strClipName.end());

		FbxTakeInfo* pTakeInfo = m_pScene->GetTakeInfo(pAnimStack->GetName());
		pAnimClip->tStartTime = pTakeInfo->mLocalTimeSpan.GetStart();
		pAnimClip->tEndTime = pTakeInfo->mLocalTimeSpan.GetStop();

		pAnimClip->eMode = m_pScene->GetGlobalSettings().GetTimeMode();
		pAnimClip->llLength = pAnimClip->tEndTime.GetFrameCount(pAnimClip->eMode) - pAnimClip->tStartTime.GetFrameCount(pAnimClip->eMode);

		m_vecAnimClip.push_back(pAnimClip);
	}
}

void CFBXLoader::Triangulate(FbxNode * _pNode)
{
	FbxNodeAttribute* pAttr = _pNode->GetNodeAttribute();

	if (pAttr &&
		(pAttr->GetAttributeType() == FbxNodeAttribute::eMesh
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs
			|| pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface))
	{
		FbxGeometryConverter converter(m_pManager);
		converter.Triangulate(pAttr, true);
	}

	int iChildCount = _pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(_pNode->GetChild(i));
	}
}

void CFBXLoader::LoadAnimationData(FbxMesh * _pMesh, tContainer * _pContainer)
{
	// Animation Data 로드할 필요가 없음
	int iSkinCount = _pMesh->GetDeformerCount(FbxDeformer::eSkin);
	if (iSkinCount <= 0 || m_vecAnimClip.empty())
		return;

	_pContainer->bAnimation = true;

	// Skin 개수만큼 반복을하며 읽는다.	
	for (int i = 0; i < iSkinCount; ++i)
	{
		FbxSkin* pSkin = (FbxSkin*)_pMesh->GetDeformer(i, FbxDeformer::eSkin);

		if (pSkin)
		{
			FbxSkin::EType eType = pSkin->GetSkinningType();
			if (FbxSkin::eRigid == eType || FbxSkin::eLinear)
			{
				// Cluster 를 얻어온다
				// Cluster == Joint == 관절
				int iClusterCount = pSkin->GetClusterCount();

				for (int j = 0; j < iClusterCount; ++j)
				{
					FbxCluster* pCluster = pSkin->GetCluster(j);

					if (!pCluster->GetLink())
						continue;

					// 현재 본 인덱스를 얻어온다.
					int iBoneIdx = FindBoneIndex(pCluster->GetLink()->GetName());
					if (-1 == iBoneIdx)
						assert(NULL);

					FbxAMatrix matNodeTransform = GetTransform(_pMesh->GetNode());

					// Weights And Indices 정보를 읽는다.
					LoadWeightsAndIndices(pCluster, iBoneIdx, _pContainer);

					// Bone 의 OffSet 행렬 구한다.
					LoadOffsetMatrix(pCluster, matNodeTransform, iBoneIdx, _pContainer);

					// Bone KeyFrame 별 행렬을 구한다.
					LoadKeyframeTransform(_pMesh->GetNode(), pCluster, matNodeTransform, iBoneIdx, _pContainer);
				}
			}
		}
	}
	CheckWeightAndIndices(_pMesh, _pContainer);
}


void CFBXLoader::CheckWeightAndIndices(FbxMesh * _pMesh, tContainer * _pContainer)
{
	vector<vector<tWeightsAndIndices>>::iterator iter = _pContainer->vecWI.begin();

	int iVtxIdx = 0;
	for (iter; iter != _pContainer->vecWI.end(); ++iter, ++iVtxIdx)
	{
		if ((*iter).size() > 1)
		{
			// 가중치 값 순으로 내림차순 정렬
			sort((*iter).begin(), (*iter).end()
				, [](const tWeightsAndIndices& left, const tWeightsAndIndices& right)
			{
				return left.dWeight > right.dWeight;
			}
			);

			double dWeight = 0.f;
			for (UINT i = 0; i < (*iter).size(); ++i)
			{
				dWeight += (*iter)[i].dWeight;
			}

			// 가중치의 합이 1이 넘어가면 처음부분에 더해준다.
			double revision = 0.f;
			if (dWeight > 1.0)
			{
				revision = 1.0 - dWeight;
				(*iter)[0].dWeight += revision;
			}

			if ((*iter).size() >= 4)
			{
				(*iter).erase((*iter).begin() + 4, (*iter).end());
			}
		}

		// 정점 정보로 변환, 
		float fWeights[4] = {};
		float fIndices[4] = {};

		for (UINT i = 0; i < (*iter).size(); ++i)
		{
			fWeights[i] = (float)(*iter)[i].dWeight;
			fIndices[i] = (float)(*iter)[i].iBoneIdx;
		}

		memcpy(&_pContainer->vecWeights[iVtxIdx], fWeights, sizeof(Vec4));
		memcpy(&_pContainer->vecIndices[iVtxIdx], fIndices, sizeof(Vec4));
	}
}

void CFBXLoader::LoadKeyframeTransform(FbxNode * _pNode, FbxCluster * _pCluster
	, const FbxAMatrix & _matNodeTransform, int _iBoneIdx, tContainer * _pContainer)
{
	if (m_vecAnimClip.empty())
		return;

	FbxVector4	v1 = { 1, 0, 0, 0 };
	FbxVector4	v2 = { 0, 0, 1, 0 };
	FbxVector4	v3 = { 0, 1, 0, 0 };
	FbxVector4	v4 = { 0, 0, 0, 1 };

	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	m_vecBone[_iBoneIdx]->matBone = _matNodeTransform;

	FbxTime::EMode eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	FbxLongLong llStartFrame = m_vecAnimClip[0]->tStartTime.GetFrameCount(eTimeMode);
	FbxLongLong llEndFrame = m_vecAnimClip[0]->tEndTime.GetFrameCount(eTimeMode);

	for (FbxLongLong i = llStartFrame; i < llEndFrame; ++i)
	{
		tKeyFrame tFrame = {};
		FbxTime   tTime = {};

		tTime.SetFrame(i, eTimeMode);

		FbxAMatrix matOffset = _pNode->EvaluateGlobalTransform(tTime) * _matNodeTransform;
		FbxAMatrix matCurTrans = matOffset.Inverse() * _pCluster->GetLink()->EvaluateGlobalTransform(tTime);
		matCurTrans = matReflect * matCurTrans * matReflect;

		tFrame.dTime = tTime.GetSecondDouble();
		tFrame.matTransform = matCurTrans;

		m_vecBone[_iBoneIdx]->vecKeyFrame.push_back(tFrame);
	}
}

void CFBXLoader::LoadOffsetMatrix(FbxCluster * _pCluster
	, const FbxAMatrix & _matNodeTransform
	, int _iBoneIdx, tContainer* _pContainer)
{
	FbxAMatrix matClusterTrans;
	FbxAMatrix matClusterLinkTrans;

	_pCluster->GetTransformMatrix(matClusterTrans);
	_pCluster->GetTransformLinkMatrix(matClusterLinkTrans);

	// Reflect Matrix
	FbxVector4 V0 = { 1, 0, 0, 0 };
	FbxVector4 V1 = { 0, 0, 1, 0 };
	FbxVector4 V2 = { 0, 1, 0, 0 };
	FbxVector4 V3 = { 0, 0, 0, 1 };

	FbxAMatrix matReflect;
	matReflect[0] = V0;
	matReflect[1] = V1;
	matReflect[2] = V2;
	matReflect[3] = V3;

	FbxAMatrix matOffset;
	matOffset = matClusterLinkTrans.Inverse() * matClusterTrans * _matNodeTransform;
	matOffset = matReflect * matOffset * matReflect;

	m_vecBone[_iBoneIdx]->matOffset = matOffset;
}


void CFBXLoader::LoadWeightsAndIndices(FbxCluster * _pCluster
	, int _iBoneIdx
	, tContainer * _pContainer)
{
	int iIndicesCount = _pCluster->GetControlPointIndicesCount();

	for (int i = 0; i < iIndicesCount; ++i)
	{
		tWeightsAndIndices tWI = {};

		// 각 정점에게 본 인덱스 정보와, 가중치 값을 알린다.
		tWI.iBoneIdx = _iBoneIdx;
		tWI.dWeight = _pCluster->GetControlPointWeights()[i];

		int iVtxIdx = _pCluster->GetControlPointIndices()[i];

		_pContainer->vecWI[iVtxIdx].push_back(tWI);
	}
}



int CFBXLoader::FindBoneIndex(string _strBoneName)
{
	wstring strBoneName = wstring(_strBoneName.begin(), _strBoneName.end());

	for (UINT i = 0; i < m_vecBone.size(); ++i)
	{
		if (m_vecBone[i]->strBoneName == strBoneName)
			return i;
	}

	return -1;
}

FbxAMatrix CFBXLoader::GetTransform(FbxNode * _pNode)
{
	const FbxVector4 vT = _pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 vR = _pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 vS = _pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}