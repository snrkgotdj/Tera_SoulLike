#ifndef _HLSL_FOG
#define _HLSL_FOG

#include "value.fx"
#include "func.fx"

//===========================
// FOG Filter Shader
// g_tex_0 : POST TARGET
// g_tex_1 : Depth Target
//===========================

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
};

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUTPUT VS_Fog(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_Fog(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float2 StepUV = float2(1 / 1600.f, 1 / 900.f);

    float4 vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float fDepth = g_tex_1.Sample(g_default_sam, _input.vUV);
 
    float4 vGray = float4(0.5f, 0.5f, 0.5f, 1.f);

    fDepth /= 20000;
    fDepth = saturate(fDepth);

    vCol = vCol * (1 - fDepth) + vGray * fDepth;

    output.vCol = vCol;

    return output;
}


#endif