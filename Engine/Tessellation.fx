#ifndef _HLSL_TESS
#define _HLSL_TESS

#include "value.fx"
//=========================
// Tessellation Shader
// g_float_0 : Came Pos Y
// g_tex_0 : Diffuse Tex
//=========================
struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
};

struct VTX_INPUT
{
    float4 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUTPUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;
};

VTX_OUTPUT VS(VTX_INPUT _input)
{
    VTX_OUTPUT output = (VTX_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos.xyz, 1.f), g_matWorld);
    output.vUV = _input.vUV;

    return output;
}

struct PatchTessFactor
{
    float arrEdge[3] : SV_TessFactor;
    float fInsideFactor : SV_InsideTessFactor;
};

PatchTessFactor ConstantHS(InputPatch<VTX_OUTPUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    PatchTessFactor output = (PatchTessFactor) 0.f;

    output.arrEdge[0] = g_float_0;
    output.arrEdge[1] = g_float_0;
    output.arrEdge[2] = g_float_0;

    output.fInsideFactor = g_float_0;

    return output;
}

struct HULL_OUTPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
[partitioning("integer")] // [partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64.f)]
[patchconstantfunc("ConstantHS")]
HULL_OUTPUT HS(InputPatch<VTX_OUTPUT, 3> _patch, uint i : SV_OutputControlPointID
            , uint _patchID : SV_PrimitiveID)
{
    HULL_OUTPUT output = (HULL_OUTPUT) 0.f;

    output.vPos = _patch[i].vPos.xyz;
    output.vUV = _patch[i].vUV;

    return output;
}

struct DOMAIN_OUTPUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DOMAIN_OUTPUT DS(PatchTessFactor _patchFactor, float3 _vUV : SV_DomainLocation
                , const OutputPatch<HULL_OUTPUT, 3> _ControlPoints)
{
    DOMAIN_OUTPUT output = (DOMAIN_OUTPUT) 0.f;

    float3 vWorldPos = _vUV.x * _ControlPoints[0].vPos + _vUV.y * _ControlPoints[1].vPos + _vUV.z * _ControlPoints[2].vPos;
    float4 vViewPos = mul(float4(vWorldPos, 1.f), g_matView);
    float2 vUV = _vUV.x * _ControlPoints[0].vUV + _vUV.y * _ControlPoints[1].vUV + _vUV.z * _ControlPoints[2].vUV;
        
    output.vPos = mul(vViewPos, g_matProj);
    output.vUV = vUV;

    return output;
}

PS_OUTPUT PS(DOMAIN_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = float4(1.f, 1.f, 1.f, 1.f);
   
    if(hasTex_0)
    {
        vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    }

    output.vCol = vCol;
    output.vCol_1 = vCol;

    return output;
}
#endif