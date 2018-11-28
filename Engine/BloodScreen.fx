#ifndef _HLSL_BLOODSCREEN
#define _HLSL_BLOODSCREEN

#include "value.fx"
#include "func.fx"

//===========================
// Gaussian Filter Shader
// g_tex_0 : POST TARGET
// g_tex_1 : Blood Texture
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

VS_OUTPUT VS_BloodScreen(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_BloodScreen(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;

    float4 vScreen = g_tex_0.Sample(g_default_sam, _input.vUV);
    float4 vBlood = g_tex_1.Sample(g_default_sam, _input.vUV);
 
    vCol.xyz = vScreen.xyz * (1 - vBlood.a * 0.5) + vBlood.xyz * vBlood.a * 0.5;

    vCol.a = 1.f;
    output.vCol = vCol;

    return output;
}


#endif