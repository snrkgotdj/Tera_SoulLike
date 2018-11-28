#include "Mesh.h"

#include "Device.h"

#include "Shader.h"
#include "ResMgr.h"
#include "InstancingBuffer.h"

CMesh::CMesh()
	: m_pVB(NULL)
	, m_iVtxCount(0)
	, m_iVtxSize(0)
	, m_eVtxUsage(D3D11_USAGE_DEFAULT)
	, m_pVtxSysMem(NULL)
	, m_iLayoutOffset(0)
	, m_iInstancingOffset(0)
	, m_pRecentBlob(NULL)
{
}

CMesh::~CMesh()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pLayout);
	SAFE_DELETE(m_pVtxSysMem);

	for (UINT i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		SAFE_RELEASE(m_vecIdxInfo[i].pIB);
		SAFE_DELETE(m_vecIdxInfo[i].pIdxSysMem);
	}
}

CMesh * CMesh::Create(UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _eVtxUsage
	, void * _pVtxSysMem, UINT _iIdxCount, UINT _iIdxSize, D3D11_USAGE _eIdxUsage
	, DXGI_FORMAT _eIdxFormat, void * _pIdxSysMem, D3D11_PRIMITIVE_TOPOLOGY _eTopology)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = _iVtxSize * _iVtxCount;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = _eVtxUsage;
	if(D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _pVtxSysMem;
	
	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return NULL;
	}

	memset(&tBufferDesc,0 , sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = _iIdxSize * _iIdxCount;

	tBufferDesc.Usage = _eIdxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	tSub.pSysMem = _pIdxSysMem;

	ID3D11Buffer* pIB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pIB)))
	{
		return NULL;
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = _iVtxCount;
	pMesh->m_iVtxSize = _iVtxSize;
	pMesh->m_eVtxUsage = _eVtxUsage;

	tIndexInfo tIdxInfo = {};
	tIdxInfo.pIB = pIB;
	tIdxInfo.iIdxCount = _iIdxCount;
	tIdxInfo.iIdxSize = _iIdxSize;
	tIdxInfo.eIdxUsage = _eIdxUsage;
	tIdxInfo.eIdxFormat = _eIdxFormat;
	pMesh->m_vecIdxInfo.push_back(tIdxInfo);

	pMesh->m_eTopology = _eTopology;

	pMesh->SetVtxSysMem(_pVtxSysMem, _iVtxSize, _iVtxCount);
	pMesh->SetIdxSysMem(_pIdxSysMem, _iIdxSize, _iIdxCount, 0);

	pMesh->AddLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	pMesh->AddLayoutDesc("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("BONECOUNT", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	return pMesh;
}

CMesh * CMesh::Create(UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _eVtxUsage, void * _pVtxSysMem, vector<tIndexInfo>& _IdxInfo, D3D11_PRIMITIVE_TOPOLOGY _eTopology)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = _iVtxSize * _iVtxCount;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = _eVtxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _pVtxSysMem;

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return NULL;
	}

	vector<tIndexInfo> vecInfo;
	for (UINT i = 0; i < _IdxInfo.size(); ++i)
	{
		tIndexInfo tIdxInfo = _IdxInfo[i];

		memset(&tBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		tBufferDesc.ByteWidth = tIdxInfo.iIdxSize * tIdxInfo.iIdxCount;

		tBufferDesc.Usage = tIdxInfo.eIdxUsage;
		if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
			tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		tSub.pSysMem = tIdxInfo.pIdxSysMem;

		if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &tIdxInfo.pIB)))
		{
			return NULL;
		}

		vecInfo.push_back(tIdxInfo);
	}

	CMesh* pMesh = new CMesh;
	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = _iVtxCount;
	pMesh->m_iVtxSize = _iVtxSize;
	pMesh->m_eVtxUsage = _eVtxUsage;
	pMesh->m_eTopology = _eTopology;
	pMesh->SetVtxSysMem(_pVtxSysMem, _iVtxSize, _iVtxCount);

	for (UINT i = 0; i < vecInfo.size(); ++i)
	{
		pMesh->m_vecIdxInfo.push_back(vecInfo[i]);
		pMesh->SetIdxSysMem(vecInfo[i].pIdxSysMem, vecInfo[i].iIdxSize, vecInfo[i].iIdxCount, i);
	}

	// Layout 정보 입력
	pMesh->AddLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	pMesh->AddLayoutDesc("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("BONECOUNT", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	return pMesh;
}

CMesh * CMesh::CreateFromContainer(CFBXLoader& _loader)
{
	const tContainer* container = &_loader.GetContainer(0);

	int iVtxCount = container->vecPos.size();

	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = sizeof(VTX) * iVtxCount;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//tBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = malloc(tBufferDesc.ByteWidth);
	VTX* pSys = (VTX*)tSub.pSysMem;
	for (int i = 0; i < iVtxCount; ++i)
	{
		pSys[i].vPos = container->vecPos[i];
		pSys[i].vUV = container->vecUV[i];
		pSys[i].vColor = Vec4(1.f, 0.f, 1.f, 0.f);
		pSys[i].vNormal = container->vecNormal[i];
		pSys[i].vTangent = container->vecTangent[i];
		pSys[i].vBinormal = container->vecBinormal[i];
		pSys[i].vWeights = container->vecWeights[i];
		pSys[i].vIndices = container->vecIndices[i];
	}

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return NULL;
	}	
	
	CMesh* pMesh = new CMesh;
	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = iVtxCount;
	pMesh->m_iVtxSize = sizeof(VTX);
	pMesh->m_eVtxUsage = tBufferDesc.Usage;
	pMesh->m_pVtxSysMem = pSys;
	pMesh->m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	// 인덱스 정보
	int iIdxBufferCount = container->vecIdx.size();

	for (int i = 0; i < iIdxBufferCount; ++i)
	{
		tIndexInfo info;
		info.eIdxFormat = INDEX32::format();
		info.iIdxSize = INDEX32::size();
		info.iIdxCount = container->vecIdx[i].size();
		info.eIdxUsage = D3D11_USAGE_DEFAULT;
		
		memset(&tBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
		tBufferDesc.ByteWidth = container->vecIdx[i].size() * INDEX32::size();

		tBufferDesc.Usage = info.eIdxUsage;
		if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
			tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		info.pIdxSysMem = malloc(tBufferDesc.ByteWidth);
		memcpy(info.pIdxSysMem, &container->vecIdx[i][0], tBufferDesc.ByteWidth);
		tSub.pSysMem = info.pIdxSysMem;
		
		if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &info.pIB)))
		{
			return NULL;
		}
		pMesh->m_vecIdxInfo.push_back(info);
	}
	
	// Layout 정보 넣기
	pMesh->AddLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);
	pMesh->AddLayoutDesc("BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0);

	/*인스턴싱 데이터는 슬롯이 다르다*/
	pMesh->AddLayoutDesc("WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WV", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("WVP", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("WVP", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	pMesh->AddLayoutDesc("ROWINDEX", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);
	pMesh->AddLayoutDesc("BONECOUNT", 0, DXGI_FORMAT_R32_SINT, 1, 1, D3D11_INPUT_PER_INSTANCE_DATA);

	if (false == container->bAnimation)
		return pMesh;

	vector<tBone*>& vecBone = _loader.GetBones();

	for (UINT i = 0; i < vecBone.size(); ++i)
	{
		tMTBone bone = {};
		bone.iDepth = vecBone[i]->iDepth;
		bone.iParentIndx = vecBone[i]->iParentIdx;
		bone.matOffset = GetMatrix(vecBone[i]->matOffset);
		bone.strBoneName = vecBone[i]->strBoneName;

		for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
		{
			tMTKeyFrame tKeyframe = {};
			tKeyframe.fTime = (float)vecBone[i]->vecKeyFrame[j].dTime;
			tKeyframe.iFrame = j;
			tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
			tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
			tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

			tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
			tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
			tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

			tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
			tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
			tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
			tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

			bone.vecKeyFrame.push_back(tKeyframe);
		}
		pMesh->m_vecBones.push_back(bone);
	}

	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strClipName;
		tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = vecAnimClip[i]->eMode;

		pMesh->m_vecAnimClip.push_back(tClip);
	}


	return pMesh;
}

void CMesh::ResetVTX(VTX * _vecVTX, UINT _iVtxCount)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = sizeof(VTX) * _iVtxCount;
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = m_eVtxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = _vecVTX;

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return;
	}

	SAFE_RELEASE(m_pVB);

	m_pVB = pVB;

}

void CMesh::ResetVTX(vector<VTX>& _vecVTX)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = sizeof(VTX) * _vecVTX.size();
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = m_eVtxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = &_vecVTX[0];

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return;
	}

	SAFE_RELEASE(m_pVB);

	m_pVB = pVB;

	SetVtxSysMem(&_vecVTX[0], sizeof(VTX), _vecVTX.size());
}

void CMesh::Reset(vector<VTX>& _vecVTX, vector<WORD>& _vecIDX)
{
	D3D11_BUFFER_DESC tBufferDesc = {};

	tBufferDesc.ByteWidth = sizeof(VTX) * _vecVTX.size();
	tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	tBufferDesc.Usage = m_eVtxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA tSub = {};
	tSub.pSysMem = &_vecVTX[0];

	ID3D11Buffer* pVB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pVB)))
	{
		return;
	}

	memset(&tBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	tBufferDesc.ByteWidth = INDEX16::size() * _vecIDX.size();
	tBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	tBufferDesc.Usage = m_vecIdxInfo[0].eIdxUsage;
	if (D3D11_USAGE_DYNAMIC == tBufferDesc.Usage)
		tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	tSub = {};
	tSub.pSysMem = &_vecIDX[0];

	ID3D11Buffer* pIB = NULL;
	if (FAILED(DEVICE->CreateBuffer(&tBufferDesc, &tSub, &pIB)))
	{
		return;
	}

	SAFE_RELEASE(m_vecIdxInfo[0].pIB);
	SAFE_DELETE(m_vecIdxInfo[0].pIdxSysMem);
	SAFE_RELEASE(m_pVB);

	m_vecIdxInfo[0].pIB = pIB;
	m_vecIdxInfo[0].iIdxCount = _vecIDX.size();

	m_pVB = pVB;

	SetVtxSysMem(&_vecVTX[0], sizeof(VTX), _vecVTX.size());
	SetIdxSysMem(&_vecIDX[0], INDEX16::size(), (UINT)_vecIDX.size(), 0);
}

void CMesh::SetVtxSysMem(void * _pVtxSysMem, UINT _iVtxSize, UINT _iVtxCount, bool _bDeleteMem)
{
	if(_bDeleteMem)
		SAFE_DELETE(m_pVtxSysMem);

	m_iVtxCount = _iVtxCount;
	m_iVtxSize = _iVtxSize;

	m_pVtxSysMem = malloc(_iVtxSize * _iVtxCount);
	memcpy(m_pVtxSysMem, _pVtxSysMem, _iVtxSize * _iVtxCount);
}

void CMesh::SetIdxSysMem(void * _pIdxSysMem, UINT _iIdxSize, UINT _iIdxCount, UINT _iIdxNumber)
{
	//SAFE_DELETE(m_vecIdxInfo[_iIdxNumber].pIdxSysMem);

	m_vecIdxInfo[_iIdxNumber].iIdxCount = _iIdxCount;
	m_vecIdxInfo[_iIdxNumber].iIdxSize = _iIdxSize;
	m_vecIdxInfo[_iIdxNumber].pIdxSysMem = malloc(_iIdxSize * _iIdxCount);
	memcpy(m_vecIdxInfo[_iIdxNumber].pIdxSysMem, _pIdxSysMem, _iIdxSize * _iIdxCount);
}

void CMesh::AddLayoutDesc(char * _pSemanticName, int _iSemanticIdx, DXGI_FORMAT _eFormat
	, int _iInputSlot, int _iInstancing, D3D11_INPUT_CLASSIFICATION _eClass)
{
	D3D11_INPUT_ELEMENT_DESC tDesc = {};

	tDesc.SemanticName = _pSemanticName;
	tDesc.SemanticIndex = _iSemanticIdx; // 같은 종류의 시멘틱에 대해서 증가하는 인덱스
	tDesc.Format = _eFormat;
	tDesc.InputSlot = _iInputSlot; // 0 ~ 15 사이의 값
	tDesc.InputSlotClass = _eClass;
	tDesc.InstanceDataStepRate = _iInstancing; // 인스턴싱

	if (0 == _iInputSlot)
	{
		tDesc.AlignedByteOffset = m_iLayoutOffset;
		m_iLayoutOffset += GetSizeofFormat(_eFormat);
	}
	else
	{
		tDesc.AlignedByteOffset = m_iInstancingOffset;
		m_iInstancingOffset += GetSizeofFormat(_eFormat);
	}

	m_vecLayoutDesc.push_back(tDesc);
}

int CMesh::SetLayout(ID3DBlob * _pVSBlob)
{
	if (_pVSBlob == m_pRecentBlob)
		return RET_SUCCESS;

	m_pRecentBlob = _pVSBlob;

	SAFE_RELEASE(m_pLayout);

	if (FAILED(DEVICE->CreateInputLayout(&m_vecLayoutDesc[0], m_vecLayoutDesc.size()
		, m_pRecentBlob->GetBufferPointer()
		, m_pRecentBlob->GetBufferSize()
		, &m_pLayout)))
	{
		return RET_FAILED;
	}
	return RET_SUCCESS;
}

void CMesh::UpdateData()
{
	UINT iOffset = 0;
	UINT iStride = m_iVtxSize;

	CONTEXT->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
	CONTEXT->IASetPrimitiveTopology(m_eTopology);
	CONTEXT->IASetInputLayout(m_pLayout);
}

void CMesh::UpdateData(D3D11_PRIMITIVE_TOPOLOGY _eTopology)
{
	UINT iOffset = 0;
	UINT iStride = m_iVtxSize;

	CONTEXT->IASetVertexBuffers(0, 1, &m_pVB, &iStride, &iOffset);
	CONTEXT->IASetPrimitiveTopology(_eTopology);
	CONTEXT->IASetInputLayout(m_pLayout);
}

void CMesh::UpdateDataInstancing()
{
	ID3D11Buffer* arrBuffer[2] = { m_pVB	, CInstancingBuffer::GetInst()->GetBuffer() };
	UINT		  iStride[2] = { m_iVtxSize	, sizeof(tInstancingData) };
	UINT		  iOffset[2] = {0, 0};	

	CONTEXT->IASetVertexBuffers(0, 2, arrBuffer, iStride, iOffset);
	CONTEXT->IASetPrimitiveTopology(m_eTopology);
	CONTEXT->IASetInputLayout(m_pLayout);
}

void CMesh::Render(UINT _iIdx)
{
	if (_iIdx >= m_vecIdxInfo.size())
		return;

	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iIdx].pIB, m_vecIdxInfo[_iIdx].eIdxFormat, 0);
	CONTEXT->DrawIndexed(m_vecIdxInfo[_iIdx].iIdxCount, 0, 0);
}

void CMesh::renderInstancing(UINT _iIdx)
{	
	CONTEXT->IASetIndexBuffer(m_vecIdxInfo[_iIdx].pIB, m_vecIdxInfo[_iIdx].eIdxFormat, 0);
	CONTEXT->DrawIndexedInstanced(m_vecIdxInfo[_iIdx].iIdxCount
		, CInstancingBuffer::GetInst()->GetInstanceCount()
		, 0, 0, 0);
}


void CMesh::Save(FILE * _pFile)
{
	CRes::Save(_pFile);

	// 정점 데이터 저장		
	fwrite(&m_iVtxSize, sizeof(int), 1, _pFile);
	fwrite(&m_iVtxCount, sizeof(int), 1, _pFile);

	int iByteSize = m_iVtxSize * m_iVtxCount;
	fwrite(&iByteSize, sizeof(int), 1, _pFile);
	fwrite(m_pVtxSysMem, 1, iByteSize, _pFile);
	fwrite(&m_eVtxUsage, sizeof(int), 1, _pFile);
	fwrite(&m_eTopology, sizeof(int), 1, _pFile);

	// 인덱스 정보
	UINT iMtrlCount = m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, _pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i].iIdxSize, sizeof(int), 1, _pFile);
		fwrite(&m_vecIdxInfo[i].iIdxCount, sizeof(int), 1, _pFile);
		fwrite(&m_vecIdxInfo[i].eIdxUsage, sizeof(int), 1, _pFile);
		fwrite(&m_vecIdxInfo[i].eIdxFormat, sizeof(int), 1, _pFile);

		iIdxBuffSize = m_vecIdxInfo[i].iIdxSize * m_vecIdxInfo[i].iIdxCount;
		fwrite(&iIdxBuffSize, sizeof(int), 1, _pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem, m_vecIdxInfo[i].iIdxSize, m_vecIdxInfo[i].iIdxCount, _pFile);
	}

	// Animation 정보 
	int iCount = m_vecAnimClip.size();
	fwrite(&iCount, sizeof(int), 1, _pFile);
	for (int i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecAnimClip[i].strAnimName, _pFile);
		fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, _pFile);
		fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, _pFile);
		fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, _pFile);
		fwrite(&m_vecAnimClip[i].eMode, sizeof(UINT), 1, _pFile);
		fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, _pFile);
		fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, _pFile);
		fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, _pFile);
	}

	iCount = m_vecBones.size();
	fwrite(&iCount, sizeof(int), 1, _pFile);

	for (int i = 0; i < iCount; ++i)
	{
		SaveWString(m_vecBones[i].strBoneName, _pFile);
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, _pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, _pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, _pFile);

		int iFrameCount = m_vecBones[i].vecKeyFrame.size();
		fwrite(&iFrameCount, sizeof(int), 1, _pFile);
		for (UINT j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
		{
			fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, _pFile);
		}
	}

	bool bHasTex = false;
	if (NULL != m_pBoneTex)
		bHasTex = true;

	fwrite(&bHasTex, sizeof(bool), 1, _pFile);

	if (bHasTex)
		SaveWString(m_pBoneTex->GetKey(), _pFile);
}

CMesh* CMesh::Load(FILE * _pFile)
{	
	wstring strKey = LoadWString(_pFile);
	wstring strPath = LoadWString(_pFile);

	// Create Mesh

	// 로드
	// 정점데이터 크기
	int iVtxSize = 0;
	fread(&iVtxSize, sizeof(int), 1, _pFile);

	int iVtxCount = 0;
	fread(&iVtxCount, sizeof(int), 1, _pFile);

	int iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, _pFile);

	void* pSysMem = malloc(iByteSize);
	fread(pSysMem, 1, iByteSize, _pFile);

	UINT eUsage = 0;
	fread(&eUsage, sizeof(int), 1, _pFile);

	UINT eTopology = 0;
	fread(&eTopology, sizeof(int), 1, _pFile);

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, _pFile);

	UINT iIdxBuffSize = 0;
	vector<tIndexInfo> vecInfo = {};
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info.iIdxSize, sizeof(int), 1, _pFile);
		fread(&info.iIdxCount, sizeof(int), 1, _pFile);
		fread(&info.eIdxUsage, sizeof(int), 1, _pFile);
		fread(&info.eIdxFormat, sizeof(int), 1, _pFile);
		fread(&iIdxBuffSize, sizeof(int), 1, _pFile);

		void* pSysMem = malloc(iIdxBuffSize);
		info.pIdxSysMem = pSysMem;

		fread(info.pIdxSysMem, info.iIdxSize, info.iIdxCount, _pFile);
		vecInfo.push_back(info);
	}

	// Animation 정보 읽기
	vector<tMTAnimClip> vecAnimClip;

	int iCount = 0;
	fread(&iCount, sizeof(int), 1, _pFile);
	vecAnimClip.resize(iCount);
	for (int i = 0; i < iCount; ++i)
	{
		vecAnimClip[i].strAnimName = LoadWString(_pFile);
		fread(&vecAnimClip[i].dEndTime, sizeof(double), 1, _pFile);
		fread(&vecAnimClip[i].dStartTime, sizeof(double), 1, _pFile);
		fread(&vecAnimClip[i].dTimeLength, sizeof(double), 1, _pFile);
		fread(&vecAnimClip[i].eMode, sizeof(UINT), 1, _pFile);
		fread(&vecAnimClip[i].iEndFrame, sizeof(int), 1, _pFile);
		fread(&vecAnimClip[i].iFrameLength, sizeof(int), 1, _pFile);
		fread(&vecAnimClip[i].iStartFrame, sizeof(int), 1, _pFile);
	}

	vector<tMTBone> vecBones;

	iCount = 0;
	fread(&iCount, sizeof(int), 1, _pFile);
	vecBones.resize(iCount);

	for (int i = 0; i < iCount; ++i)
	{
		vecBones[i].strBoneName = LoadWString(_pFile);
		fread(&vecBones[i].iDepth, sizeof(int), 1, _pFile);
		fread(&vecBones[i].iParentIndx, sizeof(int), 1, _pFile);
		fread(&vecBones[i].matOffset, sizeof(Matrix), 1, _pFile);

		int iFrameCount = 0;
		fread(&iFrameCount, sizeof(int), 1, _pFile);
		vecBones[i].vecKeyFrame.resize(iFrameCount);
		for (int j = 0; j < iFrameCount; ++j)
		{
			fread(&vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, _pFile);
		}
	}

	CMesh* pMesh = CResMgr::GetInst()->FindMesh(strKey);
	// 등록된 메쉬 없었으면 만들기
	if (NULL == pMesh)
	{
		pMesh = CMesh::Create(iVtxCount, iVtxSize, (D3D11_USAGE)eUsage, pSysMem, vecInfo, (D3D11_PRIMITIVE_TOPOLOGY)eTopology);
		pMesh->m_vecAnimClip = vecAnimClip;
		pMesh->m_vecBones = vecBones;

		CResMgr::GetInst()->AddMesh(strKey, pMesh);
	}
	
	// 불필요한 메모리 해제
	free(pSysMem);
	for (UINT i = 0; i < vecInfo.size(); ++i)
	{
		free(vecInfo[i].pIdxSysMem);
	}

	bool bHasTex = false;
	fread(&bHasTex, sizeof(bool), 1, _pFile);

	if (bHasTex)
		pMesh->m_pBoneTex = (CTexture*)CResMgr::GetInst()->Load<CTexture>(LoadWString(_pFile));

	return pMesh;
}
