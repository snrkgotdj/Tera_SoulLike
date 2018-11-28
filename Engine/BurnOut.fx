#ifndef _HLSL_DISSOLVE
#define _HLSL_DISSOLVE

#include "value.fx"
#include "Func.fx"


//========================
// BurnOut Shader
// g_tex_0 : Texture
// g_tex_1 : Normal
// g_tex_2 : ShadowMap Tex
// g_tex_3 : Noise Tex
// g_float_0 : 0 ~ 1 DeltaTime
//========================

struct VS_BURNOUT_INPUT
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
    float4 vWeight : WEIGHTS;
    float4 vIndices : BLENDINDICES;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    int iRowIdx : ROWINDEX;
    int iBoneCount : BONECOUNT;

    uint iInstanceID : SV_InstanceID;
};

struct VS_BURNOUT_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};

VS_BURNOUT_OUTPUT VS_BurnOut(VS_BURNOUT_INPUT _input)
{
    VS_BURNOUT_OUTPUT output = (VS_BURNOUT_OUTPUT) 0.f;
    
    if (g_vec4_anim.x)
    {
        Skinning(_input.vPos, _input.vTangent
        , _input.vBinormal, _input.vNormal
        , _input.vWeight, _input.vIndices, 0, _input.iBoneCount);
    }

    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;

    output.vViewPos = mul(float4(_input.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), g_matWV)).xyz;

    return output;
}

VS_BURNOUT_OUTPUT VS_BurnOut_Instancing(VS_BURNOUT_INPUT _input)
{
    VS_BURNOUT_OUTPUT output = (VS_BURNOUT_OUTPUT) 0.f;
    
    if (g_vec4_anim.x)
    {
        Skinning(_input.vPos, _input.vTangent
        , _input.vBinormal, _input.vNormal
        , _input.vWeight, _input.vIndices, _input.iRowIdx, _input.iBoneCount);
    }

    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_BurnOut(VS_BURNOUT_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    float4 vTexCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float3 vNormal = _input.vViewNormal;

    if (hasTex_1)
    {
        vNormal = g_tex_1.Sample(g_default_sam, _input.vUV).xyz;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f ---> -1.f ~ 1.f

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.xyz = mul(vNormal, matTBN);
    }
        
    for (uint i = 0; i < g_iLightCount; ++i)
    {
        tLightInfo info = CalLight(_input.vViewPos, vNormal, g_LightInfo[i]);

          // 그림자 테스트
        float3 vWorldPos = mul(float4(_input.vViewPos, 1.f), g_matInvView).xyz;

        float4 vShadowUV = mul(float4(vWorldPos, 1.f), g_LightInfo[g_int_0].matVP);
        vShadowUV.xy /= vShadowUV.w; // 투영좌표를 UV 좌표로 변환    
        vShadowUV.x = vShadowUV.x * 0.5f + 0.5f;
        vShadowUV.y = -vShadowUV.y * 0.5f + 0.5f;

         // 광원 시점에서 벋어나면
        if (vShadowUV.x < 0.f || 1.f < vShadowUV.x ||
        vShadowUV.y < 0.f || 1.f < vShadowUV.y)
        {
            vCol.rgb += vTexCol.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
        }
        else
        {
            float fDepth = g_tex_3.Sample(g_default_sam, vShadowUV.xy);
            if (fDepth * vShadowUV.w <= vShadowUV.z - 0.0001f)
            {
                // 그림자가 생긴다.
                vCol.rgb += vTexCol.rgb * ((info.tCol.vDiff.rgb * 0.3f) + info.tCol.vAmb.rgb);
            }
            else
            {
                vCol.rgb += vTexCol.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
            }
        }
    }

    if (hasTex_3)
    {
        float fNoise = g_tex_3.Sample(g_default_sam, _input.vUV).r;
        float fAlpha = 0.f;
        float fCut = 0.5f;
        if (fNoise > g_float_0)
            fAlpha = 1;
        else
            fAlpha = 0.f;

        float outline;
        if (fNoise >= g_float_0 * 1.05)
            outline = 0.f;
        else if (fNoise >= g_float_0 * 1.008)
            vCol.rgb += float3(1.f, 0.2f, 0.6f);
        else
            vCol.rgb += float3(1.f, 0.f, 0.f);
        
        vCol.a = fAlpha;
    }
    else
    {
        vCol.a = 1.f;
    }

    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}

#endif