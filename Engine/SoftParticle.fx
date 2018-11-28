#ifndef HLSL_MOTIONBLUR
#define HLSL_MOTIONBLUR

#include "value.fx"

//===================
// Standard 2D Shader
// g_tex_0 : Texture
// g_tex_1 : DepthTexture;
//===================

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float4 vProjPos : POSITION;
};

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};

VS_OUTPUT VS_SoftParticle(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    output.vProjPos = vProjPos;

    return output;
}

VS_OUTPUT VS_SoftParticle_Instancing(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    
    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    output.vProjPos = output.vPos;

    return output;
}

PS_OUTPUT PS_SoftParticle(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    
    if (g_vec4_anim.x)
    {
        float2 vUV = float2(g_vec4_uv.x + (g_vec4_uv.z * _input.vUV.x)
                          , g_vec4_uv.y + (g_vec4_uv.w * _input.vUV.y));
        vCol = g_tex_anim.Sample(g_default_sam, vUV);
    }
    else
    {
        if (hasTex_0)
        {
            vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
        }
        else
        {
            vCol = float4(1.f, 0.f, 1.f, 1.f);
        }
    }

    if (vCol.a == 0.f)
        clip(-1);

    float fWidth = 0;
    float fHeight = 0;
    g_tex_1.GetDimensions(fWidth, fHeight);
    float2 vUV = float2(_input.vPos.x / fWidth, _input.vPos.y / fHeight);
    float fDepth = g_tex_1.Sample(g_default_sam, vUV).r;
    float fTargetDepth = fDepth;
    fDepth = _input.vProjPos.z - fDepth;
    if (fDepth > 0 && fTargetDepth > 0.1)
    {
        vCol.rgb = mul(vCol.rgb, (1 - saturate(fDepth * 0.03f)));
    }

    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}




#endif