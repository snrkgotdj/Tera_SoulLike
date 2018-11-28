#ifndef _HLSL_NAVMESH
#define _HLSL_NAVMESH

#include "value.fx"

struct PS_OUTPUT
{
    float4 vDiffuse : SV_Target;
    float4 vPosition : SV_Target1;
    float4 vNormal : SV_Target2;
    float4 vDepth : SV_Target3;
};

//===========================
// NavMesh Shader 
// g_tex_0 : Diffuse Tex
// g_tex_1 : Normal Tex
// g_int_0 : Mesh Flages Tex
//===========================
struct VS_STD_INPUT
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_STD_OUTPUT
{
    float4 vPos : SV_POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vProjPos : POSITION1;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

VS_STD_OUTPUT VS_NavMesh(VS_STD_INPUT _input)
{
    VS_STD_OUTPUT output = (VS_STD_OUTPUT) 0.f;
       
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;
    output.vColor = _input.vColor;

    output.vViewPos = mul(float4(_input.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), g_matWV)).xyz;
        
    output.vProjPos = output.vPos.xyz;  
    return output;
}

VS_STD_OUTPUT VS_NavMesh_Instancing(VS_STD_INPUT _input)
{
    VS_STD_OUTPUT output = (VS_STD_OUTPUT) 0.f;
       
    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;
    output.vColor = _input.vColor;

    output.vViewPos = mul(float4(_input.vPos, 1.f), _input.matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), _input.matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), _input.matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), _input.matWV)).xyz;
        
    output.vProjPos = output.vPos.xyz;
    return output;
}


PS_OUTPUT PS_NavMesh(VS_STD_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    // Diffuse
    float4 vCol = _input.vColor;

    if (hasTex_0)
    {
        if (0.f == vCol.a)
        {
            vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
        }
    }

    output.vDiffuse.rgb = vCol.rgb;

    // View Position
    output.vPosition.xyz = _input.vViewPos;
    
    // Normal
    float3 vNormal = _input.vViewNormal;
    if (hasTex_1)
    {
        vNormal = g_tex_1.Sample(g_default_sam, _input.vUV).xyz;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f  --->  -1.f ~ 1.f
        vNormal = normalize(vNormal);

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.xyz = mul(vNormal.xyz, matTBN);
    }
    output.vNormal.xyz = vNormal;

    // Depth
    output.vDepth.x = _input.vProjPos.z;

    return output;
}

#endif