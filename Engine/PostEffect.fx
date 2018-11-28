#ifndef _HLSL_POSTEFFECT
#define _HLSL_POSTEFFECT

#include "value.fx"
#include "func.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target2;
};
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
};

VS_OUTPUT VS_POST(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;

	if (g_vec4_anim.x)
    {
        Skinning(_input.vPos, _input.vTangent
        , _input.vBinormal, _input.vNormal
        , _input.vWeight, _input.vIndices, 0, _input.iBoneCount);
    }    

    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWVP);

    output.vPos = vWorldPos;
    output.vUV = _input.vUV;

    return output;
}

VS_OUTPUT VS_POST_Inst(VS_INPUT _input)
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

    return output;
}

//==========================
// Distortion Effect Shader
// g_tex_0 : FOWARD TARGET
// g_tex_1 : Texture
// g_tex_2 : Wave Texture
// g_float_0 : DT
//==========================


PS_OUTPUT PS_E_Effect(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float4 vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

PS_OUTPUT PS_Electric_Effect(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float4 vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);
   
    _input.vUV.x = sin(_input.vUV.y * 6.28f + 3.14f * g_fDelta.y) * 0.01f + _input.vUV.x - g_fDelta.y * 0.3f;
    _input.vUV.y = cos(_input.vUV.x * 6.28f + 3.14f * g_fDelta.y) * 0.01f + _input.vUV.y + g_fDelta.y * 0.3f;

    if(hasTex_1)
        vCol = g_tex_1.Sample(g_default_sam, _input.vUV);

    //vTexCol.xyz *= 0.4f;

    vTexCol += vCol;
    vTexCol.a = g_float_0;

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

//==========================
// Distortion Effect Shader
// g_tex_0 : FOWARD TARGET
//==========================

PS_OUTPUT PS_Distortion_Effect(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    vTexUV.x = vTexUV.x;
    vTexUV.y = vTexUV.y + sin(vTexUV.y * 100) * 0.03f;

    float4 vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

//==========================
// Wave Effect Shader
// g_tex_0 : FOWARD TARGET
// g_tex_1 : Wave Texture
// g_tex_2 : Target Tex;
// g_float_0 : DT
// g_float_1 : Value
//==========================

PS_OUTPUT PS_WAVE_Effect(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float WaveA = g_tex_1.Sample(g_default_sam, (vTexUV + float2(0.0f, g_fDelta.y * 0.01f)));
    float WaveB = g_tex_1.Sample(g_default_sam, (vTexUV + float2(0.5f, -g_fDelta.y * 0.05f)));

    float WaveUV = WaveA * WaveB;
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

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

PS_OUTPUT PS_Potal_Effect(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    float fDist = distance(vTexUV, float2(0.5f, 0.5f));

    if (fDist > 0.3f)
        clip(-1);

    float2 tc = vTexUV;
    float2 p = -1.0 + 2.0 * tc;
    float len = length(p) * 10.0f;
    vTexUV = tc + (p / len) * cos(len * 5.0f + g_fDelta.y * 20.f) * 0.03f;

    float4 vTexCol = g_tex_0.Sample(g_default_sam, vTexUV);

    output.vCol = vTexCol;
    output.vCol_1 = vTexCol;

    return output;
}

//==========================
// Black Effect Shader
// g_tex_0 Texture
// g_tex_1 FOWARD TARGET
//==========================

PS_OUTPUT PS_BlackEffect(VS_OUTPUT _input) 
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    
    float fWidth;
    float fHeight;
    g_tex_1.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;

    vTexUV.x = vTexUV.x;
    vTexUV.y = vTexUV.y + sin(vTexUV.y * 100) * 0.03f;

    float4 vTexCol = g_tex_1.Sample(g_default_sam, vTexUV);
    if (g_vec4_anim.x)
    {
        float2 vUV = float2(g_vec4_uv.x + (g_vec4_uv.z * _input.vUV.x)
                          , g_vec4_uv.y + (g_vec4_uv.w * _input.vUV.y));

        vUV.x = _input.vUV.x;
        vUV.y = _input.vUV.y + sin(_input.vUV.y * 100) * 0.03f;

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
            vCol = float4(1.f, 0.f, 1.f, 0.f);
        }
    }

    if(vCol.a == 0)
        clip(-1);

    vCol.a = (vCol.r + vCol.g + vCol.b) * 0.5f;
    vCol = vTexCol * (1 - vCol.a) + vCol * vCol.a;

    output.vCol = vCol;
    output.vCol_1 = vCol;

    return output;
}

////////////////////////
// Water Effect Shader
// g_float_0 Delta Time Acc
// g_vec4_0 vCamPos
// g_tex_1 Water Texture
// g_tex_2 Normal Texture
// g_tex_3 Depth Texture
// g_texarr_7 World Texture
////////////////////////
struct VS_WATER_INPUT
{
    float3 vPos : POSITION;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_WATER_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float3 vWorldPos : POSITION;
    float3 vWorldNormal : NORMAL;

    float3 vViewPos : POSITION1;
    float3 vViewNormal : NORMAL1;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;

    float4 vProjPos : POSITION2;
};

VS_WATER_OUTPUT VS_Water(VS_WATER_INPUT _input)
{
    VS_WATER_OUTPUT output = (VS_WATER_OUTPUT) 0.f;

    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);
    output.vUV = _input.vUV;

    output.vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld).xyz;
    output.vWorldNormal = mul(float4(_input.vNormal, 1.f), g_matWorld).xyz;

    output.vViewPos = mul(float4(_input.vPos, 1.f), g_matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), g_matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), g_matWV)).xyz;

    output.vProjPos = output.vPos;

    return output;
}

PS_OUTPUT PS_Water(VS_WATER_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT)0.f;

    float4 vCol = (float4) 0.f;
    float3 vNormal = _input.vViewNormal;
    float4 vTexCol = (float4) 0.f;

    float2 vUV = (float2) 0.f;

    vUV.x = sin(_input.vUV.y * 6.28f + 3.14f * g_float_0) * 0.01f + _input.vUV.x;
    vUV.y = cos(_input.vUV.x * 6.28f + 3.14f * g_float_0) * 0.01f + _input.vUV.y + g_float_0 * 0.1f;

    _input.vUV = vUV;

    if(hasTex_1)
        vTexCol = g_tex_1.Sample(g_default_sam, _input.vUV);
    else
        vTexCol = float4(0.f, 0.f, 0.f, 1.f);
    
    if (hasTex_2)
    {
        vNormal = g_tex_2.Sample(g_default_sam, _input.vUV).xyz;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f ---> -1.f ~ 1.f

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.xyz = mul(vNormal, matTBN);
    }


   for (uint i = 0; i < g_iLightCount; ++i)
    {
        tLightInfo info = CalLight(_input.vViewPos, vNormal, g_LightInfo[i]);

        vCol.rgb += vTexCol.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
    }

    // 월드에 대한 반사
    if(hasTexArr_7)
    {
        //float3 vWorldNormal = mul(float4(vNormal, 1), g_matInvView);

        float3 vWorldEye = normalize(_input.vWorldPos - g_vec4_0.xyz);
        float3 vWorldReflect = normalize(reflect(vWorldEye, _input.vWorldNormal));
        float4 vReflectCol = g_texarr_7.Sample(g_default_sam, vWorldReflect);

        vCol.rgb = vCol.rgb * 0.5 + vReflectCol.rgb * 0.5;

    }

    vCol.a = 0.6f;
    
    output.vCol = vCol;
    output.vCol_1 = vCol;

    return output;
}

//========================
// Standard 3d Shader
// g_tex_0 : Texture
// g_tex_1 : Normal
// g_tex_2 : POST
// g_float_0 : DT;
//========================

VS_WATER_OUTPUT VS_Tile_Inst(VS_WATER_INPUT _input)
{
    VS_WATER_OUTPUT output = (VS_WATER_OUTPUT) 0.f;

    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    output.vWorldPos = mul(float4(_input.vPos, 1.f), _input.matWVP).xyz;
    output.vWorldNormal = mul(float4(_input.vNormal, 1.f), _input.matWVP).xyz;

    output.vViewPos = mul(float4(_input.vPos, 1.f), _input.matWV).xyz;
    output.vViewNormal = normalize(mul(float4(_input.vNormal, 0.f), _input.matWV)).xyz;
    output.vViewTangent = normalize(mul(float4(_input.vTangent, 0.f), _input.matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_input.vBinormal, 0.f), _input.matWV)).xyz;

    output.vProjPos = output.vPos;

    return output;
}

PS_OUTPUT PS_Tile(VS_WATER_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float fWidth;
    float fHeight;
    g_tex_2.GetDimensions(fWidth, fHeight);

    float2 vTexUV = (float2) 0.f;
    vTexUV.x = _input.vPos.x / fWidth;
    vTexUV.y = _input.vPos.y / fHeight;
  
    float4 vCol = (float4) 0.f;
    float4 vTex = g_tex_0.Sample(g_default_sam, _input.vUV);
    float4 vPostTex = g_tex_2.Sample(g_default_sam, vTexUV);
    float3 vNormal = _input.vViewNormal;

    if (hasTex_1)
    {
        vNormal = g_tex_1.Sample(g_default_sam, vTexUV).xyz;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f ---> -1.f ~ 1.f

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.xyz = mul(vNormal, matTBN);
    }
        
    for (uint i = 0; i < g_iLightCount; ++i)
    {
        tLightInfo info = CalLight(_input.vViewPos, vNormal, g_LightInfo[i]);
  
        vCol.rgb += vTex.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
    }

    if (vTex.a > 0.f)
    {
        vCol = (vCol * (1 - g_float_0)) + (vPostTex * g_float_0);
    }

    vCol.a = vTex.a;
    output.vCol = vCol;
    output.vCol_1 = vCol;

    return output;
}

#endif