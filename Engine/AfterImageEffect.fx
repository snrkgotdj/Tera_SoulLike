#ifndef _HLSL_AFTERIMAGE
#define _HLSL_AFTERIMAGE

#include "value.fx"
#include "Func.fx"


//========================
// BurnOut Shader
// g_tex_0 : Texture
// g_tex_1 : Noise Tex
// g_float_0 : 0 ~ 1 DeltaTime
//========================

struct VS_INPUT
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;
    float4 vWeight : WEIGHTS;
    float4 vIndices : BLENDINDICES;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;
    int iRowIdx : ROWINDEX;
    int iBoneCount : BONECOUNT;

    uint iInstanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_2 : SV_Target2;
};

VS_OUTPUT VS_AfterImage(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    
    if (g_vec4_anim.x)
    {
        Skinning(_input.vPos, _input.vTangent
        , _input.vBinormal, _input.vNormal
        , _input.vWeight, _input.vIndices, 0, _input.iBoneCount);
    }

    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;

    output.vViewPos = mul(float4(_input.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), g_matWV)).xyz;

    return output;
}

VS_OUTPUT VS_AfterImage_Instancing(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    
    if (_input.iRowIdx != -1)
	{
		Skinning(_input.vPos, _input.vTangent
			, _input.vBinormal, _input.vNormal
			, _input.vWeight, _input.vIndices, _input.iRowIdx, _input.iBoneCount);
	}

    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    output.vViewPos = mul(float4(_input.vPos, 1.f), _input.matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), _input.matWV)).xyz;

    return output;
}

PS_OUTPUT PS_AfterImage(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    float4 vTexCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float fAlpha = g_tex_1.Sample(g_default_sam, _input.vUV).r;
    float AddCol = (vTexCol.r + vTexCol.b + vTexCol.g);

    vCol.r = AddCol * 0.2;
    vCol.b = AddCol * 0.8;
    vCol.g = AddCol * 0.2;

    vCol.a = fAlpha * 1.5;

    if (hasTex_1)
    {
        float fNoise = g_tex_1.Sample(g_default_sam, _input.vUV).r;
        float fAlpha = 0.f;
        if (fNoise <= g_float_0)
            vCol.a = 0.f;

        float outline;
        if (fNoise >= g_float_0 * 1.05)
            outline = 0.f;
        else if (fNoise >= g_float_0 * 1.008)
            vCol.rgb += float3(0.3f, 0.3f, 1.f);
        else
            vCol.rgb = float3(0.f, 0.f, 1.f);
    }

    output.vCol = vCol;
    output.vCol_2 = vCol;

    return output;
}

#endif