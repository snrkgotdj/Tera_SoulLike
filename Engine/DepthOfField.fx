#ifndef _HLSL_DepthOfField
#define _HLSL_DepthOfField

#include "value.fx"
#include "func.fx"

//===========================
// Gaussian Filter Shader
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

VS_OUTPUT VS_DepthOfField(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_DepthOfField(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float2 StepUV = float2(1 / 1600.f, 1 / 900.f);

    float4 vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float fDepth = g_tex_1.Sample(g_default_sam, _input.vUV);
 
    int iLevel = fDepth / 500;

    if (iLevel >= 8)
        iLevel = 8;

    vCol = BlurColor(g_tex_0, _input.vUV, StepUV, iLevel);

    output.vCol = vCol;

    return output;
}




#endif