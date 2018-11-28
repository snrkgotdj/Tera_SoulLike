#ifndef _HLSL_HDR
#define _HLSL_HDR

#include "value.fx"

Texture2D<float4> HDRTex : register(t0);
StructuredBuffer<float> AvgLum : register(t1);
Texture2D<float4> BloomTex : register(t2);

SamplerState PointSampler : register(s0);
SamplerState LinearSampler : register(s1);

static const float2 arrBasePos[4] =
{
    float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

static const float2 arrUV[4] =
{
    float2(0.0, 0.0),
	float2(1.0, 0.0),
	float2(0.0, 1.0),
	float2(1.0, 1.0),
};

//-----------------------------------------------------------------------------------------
// Vertex shader
//-----------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol1 : SV_Target1;
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

VS_OUTPUT VS_HDR(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

//-----------------------------------------------------------------------------------------
// Pixel shader
//-----------------------------------------------------------------------------------------

cbuffer FinalPassConstants : register(b2)
{
	// Tone mapping
    float MiddleGrey : packoffset(c0);
    float LumWhiteSqr : packoffset(c0.y);
    float BloomScale : packoffset(c0.z);
}

static const float3 LUM_FACTOR = float3(0.299, 0.587, 0.114);

float3 ToneMapping(float3 HDRColor)
{
	// Find the luminance scale for the current pixel
    float LScale = dot(HDRColor, LUM_FACTOR);
    LScale *= MiddleGrey / AvgLum[0];
    LScale = (LScale + LScale * LScale / LumWhiteSqr) / (1.0 + LScale);
	
	// Apply the luminance scale to the pixels color
    return HDRColor * LScale;
}

PS_OUTPUT PS_HDR(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

	// Get the color sample
    float3 color = HDRTex.Sample(PointSampler, _input.vUV);

    // Add the bloom contribution
    color += BloomScale * BloomTex.Sample(LinearSampler, _input.vUV);

	// Tone mapping
    color = ToneMapping(color);

	// Output the LDR value
    output.vCol = float4(color, 1.0f);
    output.vCol1 = float4(color, 1.0f);

    return output;
}

#endif