#ifndef _HLSL_LIGHT
#define _HLSL_LIGHT

#include "value.fx"
#include "Func.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
};

//==============
// Volume Shader
//==============
struct VS_INPUT
{
    float3 vPos : POSITION;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
};

VS_OUTPUT VS_Volume(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    return output;
}

PS_OUTPUT PS_Volume(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    return output;
}

//===============================
// Light Draw Shader
// g_tex_0 : Position Target Tex
// g_tex_1 : Normal Target Tex
// g_tex_2 : Depth Target Tex
// g_tex_3 : ShadowMap Tex
// g_tex_4 : Specular Target Tex
// g_tex_5 : Emissive Target Tex
// g_int_0 : Light Idx
//===============================
struct VS_LD_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_LD_OUTPUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_LD_OUTPUT VS_LightDraw(VS_LD_INPUT _input)
{
    VS_LD_OUTPUT output = (VS_LD_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_LightDraw(VS_LD_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    float fDepth = g_tex_2.Sample(g_default_sam, _input.vUV).x;
    if (fDepth <= 0.01f)
        clip(-1);

    float3 vViewPos = g_tex_0.Sample(g_default_sam, _input.vUV).rgb;
    float3 vNormal = g_tex_1.Sample(g_default_sam, _input.vUV).rgb;
    float4 vSpecular = g_tex_4.Sample(g_default_sam, _input.vUV);
    float3 vEmissive = g_tex_5.Sample(g_default_sam, _input.vUV);
    
    tLightInfo info = CalLight(vViewPos, vNormal, g_LightInfo[g_int_0]);

    // 그림자 테스트
    float3 vWorldPos = mul(float4(vViewPos, 1.f), g_matInvView).rgb;

    float4 vShadowUV = mul(float4(vWorldPos, 1.f), g_LightInfo[g_int_0].matVP);
    vShadowUV.xy /= vShadowUV.w; // 투영좌표를 UV 좌표로 변환    
    vShadowUV.x = vShadowUV.x * 0.5f + 0.5f;
    vShadowUV.y = -vShadowUV.y * 0.5f + 0.5f;

     // 광원 시점에서 벋어나면
    if (vShadowUV.x < 0.f || 1.f < vShadowUV.x ||
        vShadowUV.y < 0.f || 1.f < vShadowUV.y)
    {
        output.vCol.rgb = info.tCol.vDiff.rgb + info.tCol.vAmb.rgb;
        output.vCol_1 = vSpecular + info.tCol.vSpec;
    }
    else
    {
        float fShaderDepth = g_tex_3.Sample(g_default_sam, vShadowUV.xy);

        if ((fShaderDepth > 0.f) && fShaderDepth * vShadowUV.w < vShadowUV.z - 0.002f)
        {
            // 그림자가 생긴다.
            output.vCol.rgb = info.tCol.vDiff.rgb * 0.4f + info.tCol.vAmb.rgb;
            output.vCol_1 = vSpecular;
        }
        else
        {
            output.vCol.rgb = info.tCol.vDiff.rgb + info.tCol.vAmb.rgb;
            output.vCol_1 = vSpecular + info.tCol.vSpec;
        }
    }
    output.vCol.rgb += vEmissive.rgb;
    return output;
}
// Light Draw Shader 가 사용할 빛끼리 가산혼합하는 블렌드 스테이트 필요
// CShaderMgr 에 Light Draw Shader 만들기
// LightMtrl 에 셋팅

//==================================
// Light Merge Shader
// g_tex_0 : Diffuse Target Tex
// g_tex_1 : Diffuse Light Target
// g_tex_2 : Specular Light Target
//==================================
struct PS_MergeOUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};

VS_LD_OUTPUT VS_LightMerge(VS_LD_INPUT _input)
{
    VS_LD_OUTPUT output = (VS_LD_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_MergeOUTPUT PS_LightMerge(VS_LD_OUTPUT _input)
{
    PS_MergeOUTPUT output = (PS_MergeOUTPUT) 0.f;
    
    float4 vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float3 vLight = g_tex_1.Sample(g_default_sam, _input.vUV).xyz;
    float3 vSpec = g_tex_2.Sample(g_default_sam, _input.vUV).xyz;
    
    if (vCol.a != 0.f)
    {
        output.vCol.rgb = float3(0.2f, 0.8f, 0.2f) * vCol.a;

    }
    output.vCol.rgb += vCol.rgb * vLight + vSpec;
    output.vCol.a = 1.f;

    output.vCol_1 = output.vCol;
    output.vCol_2 = output.vCol;

    return output;
}

struct VS_SM_INPUT
{
    float3 vPos : POSITION;
    float4 vWeight : WEIGHTS;
    float4 vIndices : BLENDINDICES;

    row_major matrix matWVP : WVP;
    int iRowIdx : ROWINDEX;
    int iBoneCount : BONECOUNT;

    uint iInstanceID : SV_InstanceID;
};

struct VS_SM_OUTPUT
{
    float4 vPos : SV_Position;
    float4 vProjPos : POSITION;
};

VS_SM_OUTPUT VS_DirShadowMap(VS_SM_INPUT _input)
{
    VS_SM_OUTPUT output = (VS_SM_OUTPUT) 0.f;

    if (g_vec4_anim.x)
    {
        Skinning(_input.vPos, _input.vWeight, _input.vIndices, 0, _input.iBoneCount);
    }

    output.vPos = mul(mul(float4(_input.vPos, 1.f), g_matWorld), g_LightInfo[g_int_0].matVP);
    output.vProjPos = output.vPos;

    return output;
}

VS_SM_OUTPUT VS_DirShadowMap_Instancing(VS_SM_INPUT _input)
{
    VS_SM_OUTPUT output = (VS_SM_OUTPUT) 0.f;

    if (_input.iRowIdx != -1)
    {
        Skinning(_input.vPos, _input.vWeight, _input.vIndices, _input.iRowIdx, _input.iBoneCount);
    }

    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vProjPos = output.vPos;

    return output;
}

float PS_DirShadowMap(VS_SM_OUTPUT _input) : SV_Target
{
    float fDepth = 0.f;   
    fDepth = _input.vProjPos.z / _input.vProjPos.w;

    return fDepth;
}

#endif