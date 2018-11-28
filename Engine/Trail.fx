#ifndef _HLSL_TRAIL
#define _HLSL_TRAIL

#include "value.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target2;
};

//======================
// Trail Shader
// g_tex_0 Render Target 1
// g_tex_1 Trail Image
//======================
struct VS_COL_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    float4 vCol : COLOR;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_COL_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
    float4 vCol : COLOR;
};

VS_COL_OUTPUT VS_Trail(VS_COL_INPUT _input)
{
    VS_COL_OUTPUT output = (VS_COL_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;
    output.vCol = _input.vCol;

    return output;
}

VS_COL_OUTPUT VS_Trail_Instancing(VS_COL_INPUT _input)
{
    VS_COL_OUTPUT output = (VS_COL_OUTPUT) 0.f;
    
    float4 vProjPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vPos = vProjPos;
    output.vUV = _input.vUV;
    output.vCol = _input.vCol;

    return output;
}

PS_OUTPUT PS_Trail(VS_COL_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
   
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float WaveA = g_tex_2.Sample(g_default_sam, (vTexUV + float2(0.0f, g_float_0 * 0.01f)));
    float WaveB = g_tex_2.Sample(g_default_sam, (vTexUV + float2(0.5f, -g_float_0 * 0.05f)));

    float2 WaveUV = WaveA * WaveB;
    WaveUV *= 0.11f;
    float2 vTestUV = vTexUV + WaveUV;
    float4 vTexCol = (float4) 0.f;
    if (vTestUV.x >= 1.f || vTestUV.y >= 1.f)
    {
        vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);
    }
    else
    {
        vTexCol = g_tex_0.Sample(g_default_sam, vTexUV + WaveUV);
    }
    
    float4 vTrailCol = (float) 0.f;
    if (hasTex_1)
    {
        vTrailCol = g_tex_1.Sample(g_default_sam, _input.vUV.xy);
    }

    vTexCol += vTrailCol * _input.vCol.a * 1.4 * _input.vCol.a;

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

PS_OUTPUT PS_Trail_Alpha(VS_COL_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
   
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float4 vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);
 
    
    float4 vTrailCol = (float) 0.f;
    if (hasTex_1)
    {
        vTrailCol = g_tex_1.Sample(g_default_sam, _input.vUV.xy);
    }

    vTexCol += vTrailCol * _input.vCol.a * 1.4 * _input.vCol.a;

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

#endif