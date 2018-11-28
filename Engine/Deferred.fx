#ifndef _HLSL_DEFERRED
#define _HLSL_DEFERRED

#include "value.fx"
#include "func.fx"

struct PS_OUTPUT
{
    float4 vDiffuse : SV_Target;
    float4 vPosition : SV_Target1;
    float4 vNormal : SV_Target2;
    float4 vDepth : SV_Target3;
    float4 vSpecular : SV_Target4;
    float4 vVelocity : SV_Target5;
    float4 vEmissive : SV_Target6;
};

//=================
// Standard Shader 
// g_tex_0 : Diffuse Tex
// g_tex_1 : Normal Tex
// g_tex_2 : Specular Tex
// g_tex_3 : Emissive Tex
//=================
struct VS_STD_INPUT
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

struct VS_STD_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vProjPos : POSITION1;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;

    float4 vDir : DIRECTION;
};

VS_STD_OUTPUT VS_Std_Deferred(VS_STD_INPUT _input)
{
    VS_STD_OUTPUT output = (VS_STD_OUTPUT) 0.f;

    float4 vPrevPos = (float4) 0.f;
    vPrevPos.xyz = _input.vPos;

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
        
    output.vProjPos = output.vPos.xyz;

    // º”µµ
    if (g_vec4_anim.y)
    {
        Skinning_Prev(vPrevPos.xyz, _input.vWeight, _input.vIndices, 0, _input.iBoneCount);
        vPrevPos = mul(float4(vPrevPos.xyz, 1.f), g_matBeforeWVP);
        float3 vDir = output.vPos.xyz - vPrevPos.xyz;
        float2 vVelocity = output.vPos.xy / output.vPos.w  - vPrevPos.xy / vPrevPos.w;

        float fDot = dot(normalize(vDir), _input.vNormal);
        if(fDot < 0.0f)
        {
            output.vPos = vPrevPos;
        }

        vDir.xy = vVelocity;
        vDir.y *= -1;
        output.vDir.xy = vVelocity;
        output.vDir.z = output.vPos.z;
        output.vDir.w = output.vPos.w;
    }

    return output;
}

VS_STD_OUTPUT VS_Std_Deferred_Instancing(VS_STD_INPUT _input)
{
    VS_STD_OUTPUT output = (VS_STD_OUTPUT) 0.f;

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
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), _input.matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), _input.matWV)).xyz;
        
    output.vProjPos = output.vPos.xyz;

    return output;
}


PS_OUTPUT PS_Std_Deferred(VS_STD_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    // Diffuse
    float4 vCol = float4(1.f, 0.f, 1.f, 1.f);

    if (hasTex_0)
    {
        vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    }

    // View Position
    output.vPosition.rgb = _input.vViewPos;
    
    // Normal
    float3 vNormal = _input.vViewNormal;
    if (hasTex_1)
    {
        vNormal = g_tex_1.Sample(g_default_sam, _input.vUV).rgb;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f  --->  -1.f ~ 1.f
        vNormal = normalize(vNormal);

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.rgb = mul(vNormal.rgb, matTBN);
    }
    output.vNormal.rgb = vNormal;

    float3 vDir = normalize(-_input.vViewPos);
    float fDot = smoothstep(0, 1, dot(vNormal, vDir));
    float fRim = 1.f - fDot;

    float3 vRimCol = float3(1.f, 1.f, 1.f);

    if(1.f == g_fDelta.z) // ∫∏Ω∫ ««∞›
    {
        vRimCol = float3(0.9f, 0.2f, 0.2f) * 3.f;
    }
    else if (2.f == g_fDelta.z) // ∫∏Ω∫ ±√±ÿ±‚ ¡ÿ∫Ò¥‹∞Ë
    {
        vRimCol *= 3.f;
    }
    else if (3.f == g_fDelta.z) // ∫∏Ω∫ ±√±ÿ±‚
    {
        vRimCol *= 10.f;
    }
    else if (4.f == g_fDelta.z) // ∞Ò∑Ω
    {
        vRimCol *= 0.5f;
    }
    else if (0.f == g_fDelta.z)
    {
        vRimCol *= 1.5f;
    }

    output.vDiffuse.rgb = vCol.rgb + (pow(fRim, 2.f) * vRimCol);

    // Specular
    if(hasTex_2)
    {
        output.vSpecular = g_tex_2.Sample(g_default_sam, _input.vUV);
    }
    else
    {
        output.vSpecular = (float4) 0.f;
    }

    output.vDepth.x = _input.vProjPos.z;

    // Velocity
    output.vVelocity = _input.vDir;

    // Emissive
    if(hasTex_3)
    {
        output.vEmissive = g_tex_3.Sample(g_default_sam, _input.vUV);
    }
    else
    {
        output.vEmissive = (float4) 0.f;
    }

    return output;
}

#endif