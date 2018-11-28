#ifndef HLSL_MOTIONBLUR
#define HLSL_MOTIONBLUR

#include "value.fx"

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

//===========================
// Gaussian Filter Shader
// g_tex_0 : POST TARGET
// g_tex_1 : VELOCITY TARGET
//===========================
VS_OUTPUT VS_MOTIONBLUR(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_MOTIONBLUR(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    int SampleLevel = 5;
    float4 vCol = (float4) 0.f;
    vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float4 vVelocity = g_tex_1.Sample(g_default_sam, _input.vUV);

    if (0 != vVelocity.x && 0 != vVelocity.y)
    {
        int i = 0;
        for (; i < SampleLevel; ++i)
        {
            //vCol += float4(1.f, 0.f, 0.f, 1.f);
            vCol += g_tex_0.Sample(g_default_sam, _input.vUV + i * vVelocity.xy * 10);
        }
        vCol /= i + 1;
        //vCol = g_tex_1.Sample(g_default_sam, _input.vUV);
    }

    output.vCol = vCol;
    return output;
}

#endif