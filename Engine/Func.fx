#ifndef _HLSL_FUNC
#define _HLSL_FUNC

#include "value.fx"

// 전역 함수
tLightInfo CalLight(float3 _vViewPos, float3 _vViewNormal, tLightInfo _tLightInfo)
{
    tLightInfo info = (tLightInfo) 0.f;

    float3 vLight = (float3) 0.f;
    float fFactor = 1.f;
    float cosAtt = 1.f;

    // Direction Light
    if (_tLightInfo.iLightType == 0)
    {
        vLight = mul(float4(_tLightInfo.vLightDir, 0.f), g_matView).xyz;
        vLight = normalize(-vLight);
                
        // 주변광
        info.tCol.vAmb = _tLightInfo.tCol.vAmb;
    }
    // Point Light
    else if (_tLightInfo.iLightType == 1)
    {
        float3 vLightPos = mul(_tLightInfo.vLightPos, g_matView).xyz;
        vLight = normalize(vLightPos - _vViewPos); // 표면에서 광원을 향한 방향

        float fRange = length(_tLightInfo.vLightDir);
        float fDist = distance(vLightPos, _vViewPos);
        fFactor = saturate((fRange - fDist) / fRange);
    }
    // Spot Light
    else
    {
        float3 vLightPos = mul(_tLightInfo.vLightPos, g_matView).xyz;
        vLight = normalize(vLightPos - _vViewPos);
        float3 vLightDIr = -mul(float4(_tLightInfo.vLightDir, 0.f), g_matView).xyz;
        vLightDIr = normalize(vLightDIr);


        float fRange = length(_tLightInfo.vLightDir);
        float fDist = distance(vLightPos, _vViewPos);
        fFactor = saturate((fRange - fDist) / fRange);

        //float cosAng = dot(vLight, vLightDIr);
        //cosAtt = saturate((cosAng - _tLightInfo.vSpotAngle.x) * (1 / _tLightInfo.vSpotAngle.y));
        //cosAtt *= cosAtt;

        float minCos = _tLightInfo.vSpotAngle.x;
        float maxCos = _tLightInfo.vSpotAngle.y; //(minCos + 1.f) / 2.f;
        float cosAng = dot(vLight, vLightDIr);
        cosAtt = smoothstep(minCos, maxCos, cosAng);
    }
       
    info.tCol.vDiff = _tLightInfo.tCol.vDiff * saturate(dot(_vViewNormal, normalize(vLight))) * fFactor * cosAtt;

    if (fFactor > 0)
    {
        // 반사강도 구하기
        vLight = normalize(vLight);

        // 반사벡터 구하기
        float3 vReflect = normalize(2.f * dot(vLight, _vViewNormal) * _vViewNormal - vLight);
        float3 vEye = normalize(-_vViewPos);
        info.tCol.vSpec = _tLightInfo.tCol.vSpec * pow(saturate(dot(vEye, vReflect)), 10) * fFactor;

    }
    return info;
}

// Tessellation Level
float CalTessLvl(float _fMinDist, float _fMaxDist, int _iMaxLevel, int _iMinLevel, float3 _vWorldPos)
{
    float3 vDist = g_vec4_2.xyz - _vWorldPos;
    vDist.y = 0.f;
    float fDist = length(vDist);

    float fRatio = (fDist - _fMinDist) / (_fMaxDist - _fMinDist);
    
    return pow(2, saturate(floor(lerp(4, 0, fRatio)) / 4.f) * 4.f);
}

matrix GetBoneMat(int _iBoneIdx, int _iRowIdx, int _iBoneCnt)
{
    matrix matBone =
    {
        g_tex_bone.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx), 0, 0)),
        g_tex_bone.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 1, 0, 0)),
        g_tex_bone.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 2, 0, 0)),
        g_tex_bone.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 3, 0, 0))
    };
    return matBone;
}

matrix GetBoneMat_Prev(int _iBoneIdx, int _iRowIdx, int _iBoneCnt)
{
    matrix matBone =
    {
        g_tex_bone_prev.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx), 0, 0)),
        g_tex_bone_prev.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 1, 0, 0)),
        g_tex_bone_prev.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 2, 0, 0)),
        g_tex_bone_prev.Load(int3(4 * (_iBoneIdx + _iBoneCnt * _iRowIdx) + 3, 0, 0))
    };
    return matBone;
}

void Skinning(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
                        , inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx, int _iBoneCnt)
{
    float3 vPos = 0.f;
    float3 vTangent = 0.f;
    float3 vBinormal = 0.f;
    float3 vNormal = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx, _iBoneCnt);

        vPos += mul(float4(_vPos, 1.f), matBone).xyz * _vWeight[i];
        vTangent += mul(float4(_vTangent, 0.f), matBone).xyz * _vWeight[i];
        vBinormal += mul(float4(_vBinormal, 0.f), matBone).xyz * _vWeight[i];
        vNormal += mul(float4(_vNormal, 0.f), matBone).xyz * _vWeight[i];
    }
    
    _vPos = vPos;
    _vTangent = normalize(vTangent);
    _vBinormal = normalize(vBinormal);
    _vNormal = normalize(vNormal);
}

void Skinning_Prev(inout float3 _vPos, inout float3 _vTangent, inout float3 _vBinormal, inout float3 _vNormal
                        , inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx, int _iBoneCnt)
{
    float3 vPos = 0.f;
    float3 vTangent = 0.f;
    float3 vBinormal = 0.f;
    float3 vNormal = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat_Prev((int) _vIndices[i], _iRowIdx, _iBoneCnt);

        vPos += mul(float4(_vPos, 1.f), matBone).xyz * _vWeight[i];
        vTangent += mul(float4(_vTangent, 0.f), matBone).xyz * _vWeight[i];
        vBinormal += mul(float4(_vBinormal, 0.f), matBone).xyz * _vWeight[i];
        vNormal += mul(float4(_vNormal, 0.f), matBone).xyz * _vWeight[i];
    }
    
    _vPos = vPos;
    _vTangent = normalize(vTangent);
    _vBinormal = normalize(vBinormal);
    _vNormal = normalize(vNormal);
}


void Skinning(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx, int _iBoneCnt)
{
    float3 vPos = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat((int) _vIndices[i], _iRowIdx, _iBoneCnt);

        vPos += mul(float4(_vPos, 1.f), matBone).xyz * _vWeight[i];
    }
    
    _vPos = vPos;
}

void Skinning_Prev(inout float3 _vPos, inout float4 _vWeight, inout float4 _vIndices, int _iRowIdx, int _iBoneCnt)
{
    float3 vPos = 0.f;

    for (int i = 0; i < 4; ++i)
    {
        if (0.f == _vWeight[i])
            continue;

        matrix matBone = GetBoneMat_Prev((int) _vIndices[i], _iRowIdx, _iBoneCnt);

        vPos += mul(float4(_vPos, 1.f), matBone).xyz * _vWeight[i];
    }
    
    _vPos = vPos;
}

float4 BlurColor(Texture2D _pTex, float2 _vUV, float2 _vUVstep, int _iLevel)
{
    float4 vCol = (float4) 0.f;
    int iSum = 0;
    vCol += g_tex_0.Sample(g_default_sam, _vUV) * g_vGaussian[4];
    iSum += g_vGaussian[4];
    for (int i = 1; i < _iLevel; ++i)
    {
        if( i == 4)
            continue;

        float2 vUV = _vUV + float2((i - 2) % 3 * _vUVstep.x, (((i - 1) / 3) - 1) * _vUVstep.y);
        vCol += g_tex_0.Sample(g_default_sam, vUV) * g_vGaussian[i];
        iSum += g_vGaussian[i];
    }

     
    vCol /= iSum;
    

    //else if(_iLevel <= 24)
    //{
    //    for (int i = 0; i < (float) _iLevel / (float) 5; i++)
    //    {
    //        for (int j = 0; j < _iLevel % 5; j++)
    //        {
    //            float2 vUV = _vUV + float2((i - 1) * _vUVstep.x, (j - 1) * _vUVstep.y);
    //            vCol += g_tex_0.Sample(g_default_sam, vUV) * g_vGaussian2[i * 5 + j];
    //            iSum += g_vGaussian2[i * 5 + j];
    //        }
    //    }
    //    vCol /= iSum;
    //}

    return vCol;
}

#endif