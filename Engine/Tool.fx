#ifndef _HLSL_TOOL
#define _HLSL_TOOL

#include "value.fx"

//=========================
// UI Shader
// g_tex_0 : Target Texture
//=========================
struct VS_INPUT
{
    float4 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUTPUT VS_UI(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    
    matrix matProj = matrix(g_vec4_0, g_vec4_1, g_vec4_2, g_vec4_3);

    output.vPos = mul(_input.vPos, g_matWorld);
    output.vPos = mul(output.vPos, matProj);
    output.vUV = _input.vUV;

    return output;
}

float4 PS_UI(VS_OUTPUT _input) : SV_Target
{
    float4 vCol = (float4) 0.f;

    vCol = g_tex_0.Sample(g_default_sam, _input.vUV);

    return vCol;
}


//===================
// Grid Shader
// g_int_0 : Grid Step
// g_float_0 : Grid Alpha
// g_float_1 : Grid Bold
//===================
struct VS_GRID_OUTPUT
{
    float4 vPos : SV_POSITION;
    float3 vWorldPos : POSITION;
};

VS_GRID_OUTPUT VS_Grid(float3 _vLocalPos : POSITION)
{
    VS_GRID_OUTPUT output = (VS_GRID_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_vLocalPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vWorldPos = vWorldPos.xyz;

    return output;
}

float4 PS_Grid(VS_GRID_OUTPUT _input) : SV_Target
{
    float4 vCol = (float4) 0.f;
  
    float fX = (float) abs((int) _input.vWorldPos.x % (g_int_0 * 10));
    float fZ = (float) abs((int) _input.vWorldPos.z % (g_int_0 * 10));
    
    float fStep = (float) g_int_0;

    if (fStep * 10 - g_float_1 <= fX || fX <= g_float_1
        || fStep * 10 - g_float_1 <= fZ || fZ <= g_float_1)
    {
        vCol = float4(1.f, 1.f, 1.f, 1.f);
        return vCol;
    }

    fX = (float) abs((int) _input.vWorldPos.x % g_int_0);
    fZ = (float) abs((int) _input.vWorldPos.z % g_int_0);

    if (fStep - g_float_1 <= fX || fX <= g_float_1
        || fStep - g_float_1 <= fZ || fZ <= g_float_1)
    {
        vCol = float4(1.f, 1.f, 1.f, g_float_0);
        return vCol;
    }
     
    clip(-1);

    return vCol;
}

//==================================
// Light Range Sphere Shader Shader
// g_float_0 : Scale
//==================================
struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
};


struct VS_COL_INPUT
{
    float3 vPos : POSITION;
    float4 vCol : COLOR;
};


struct VS_COL_OUTPUT
{
    float4 vPos : SV_POSITION;
    float4 vCol : COLOR;
    float4 vLocalPos : POSITION;
};

VS_COL_OUTPUT VS_LR_Sphere(VS_COL_INPUT _input)
{
    VS_COL_OUTPUT output = (VS_COL_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos.xyz, 1.f), g_matWVP);

    output.vLocalPos = float4(_input.vPos.xyz, 1.f);
    output.vCol = _input.vCol;

    return output;
}

PS_OUTPUT PS_LR_Sphere(VS_COL_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float fDist = distance(float3(0.f, 0.f, 0.f), _input.vLocalPos.xyz);

    output.vCol = _input.vCol;
    output.vCol_1 = _input.vCol;

    if (fDist > 0.99f)
    {
        output.vCol.a = 1;
        output.vCol_1.a = 1;
    }
    else
    {
        output.vCol.a = 0;
        output.vCol_1.a = 0;
    }

    return output;
}
#endif