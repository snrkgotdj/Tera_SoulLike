#ifndef _HLSL_UI
#define _HLSL_UI

#include "value.fx"

struct PS_OUTPUT
{
    float4 vCol_0 : SV_Target;
    float4 vCol_1 : SV_Target1;
};

//================
// Button Shader
// g_tex_0
//================
struct VS_BUTTON_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_BUTTON_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_BUTTON_OUTPUT VS_BUTTON(VS_BUTTON_INPUT _input)
{
    VS_BUTTON_OUTPUT output = (VS_BUTTON_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    return output;
}


VS_BUTTON_OUTPUT VS_BUTTON_Instancing(VS_BUTTON_INPUT _input)
{
    VS_BUTTON_OUTPUT output = (VS_BUTTON_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), _input.matWVP);

    output.vPos = vWorldPos;
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_BUTTON(VS_BUTTON_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = float4(0.5f, 0.5f, 0.5f, 1.f);
    float3 vAddCol = (float3) 0.f;

    if(hasTex_0)
    {
        vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    }

    if(1 == g_int_0)
    {
        vCol.rgb += 0.2f;
    }
    else if(2 == g_int_0)
    {
        vCol.rgb -= 0.2f;
    }
 
    vCol.a = 1.f;
    output.vCol_0 = vCol;
    output.vCol_1 = vCol;

    return output;
}

#endif