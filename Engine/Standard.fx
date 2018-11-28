#ifndef _HLSL_STANDARD
#define _HLSL_STANDARD

#include "value.fx"
#include "Func.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};


//====================
// Vertex Color Shader
//====================
struct VS_COL_INPUT
{
    float3 vPos : POSITION;
    float4 vCol : COLOR;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};


struct VS_COL_OUTPUT
{
    float4 vPos : SV_POSITION;   
    float4 vCol : COLOR;
};

VS_COL_OUTPUT VS_Color(VS_COL_INPUT _input)
{
    VS_COL_OUTPUT output = (VS_COL_OUTPUT) 0.f;

    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);
    
    output.vPos = vProjPos;
    output.vCol = _input.vCol;

    return output;
}

VS_COL_OUTPUT VS_Color_Instancing(VS_COL_INPUT _input)
{
    VS_COL_OUTPUT output = (VS_COL_OUTPUT) 0.f;
    
    float4 vProjPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
        
    output.vPos = vProjPos;
    output.vCol = _input.vCol;

    return output;
}

PS_OUTPUT PS_Color(VS_COL_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
   
    output.vCol = _input.vCol;
    output.vCol_1 = _input.vCol;
    output.vCol_2 = _input.vCol;

    return output;
}

//================
// Texture Shader
// g_tex_0
//================
struct VS_TEX_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_TEX_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_TEX_OUTPUT VS_Tex(VS_TEX_INPUT _input)
{
    VS_TEX_OUTPUT output = (VS_TEX_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    return output;
}


VS_TEX_OUTPUT VS_Tex_Instancing(VS_TEX_INPUT _input)
{
    VS_TEX_OUTPUT output = (VS_TEX_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), _input.matWVP);

    output.vPos = vWorldPos;
    output.vUV = _input.vUV;    

    return output;
}

PS_OUTPUT PS_Tex(VS_TEX_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
   
    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}

//================
// Collider Shader
// g_int_0
//================
struct VS_COLLIDER_OUTPUT
{
    float4 vPos : SV_POSITION;
};

VS_COLLIDER_OUTPUT VS_Collider(float3 vPos : POSITION)
{
    VS_COLLIDER_OUTPUT output = (VS_COLLIDER_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;

    return output;
}

VS_COLLIDER_OUTPUT VS_Collider_Instancing(VS_TEX_INPUT _input)
{
    VS_COLLIDER_OUTPUT output = (VS_COLLIDER_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vPos = vWorldPos;

    return output;
}

PS_OUTPUT PS_Collider(VS_COLLIDER_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    if (g_int_0)
    {
        output.vCol = float4(1.f, 0.f, 0.f, 1.f);
        output.vCol_1 = float4(1.f, 0.f, 0.f, 1.f);
        output.vCol_2 = float4(1.f, 0.f, 0.f, 1.f);
    }
    else
    {
        output.vCol = float4(0.f, 1.f, 0.f, 1.f);
        output.vCol_1 = float4(0.f, 1.f, 0.f, 1.f);
        output.vCol_2 = float4(0.f, 1.f, 0.f, 1.f);
    }
  
    return output;
}

//===================
// Standard 2D Shader
// g_tex_0 : Texture
//===================

struct VS_STD_2D_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_STD_2D_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

VS_STD_2D_OUTPUT VS_Std_2D(VS_STD_2D_INPUT _input)
{
    VS_STD_2D_OUTPUT output = (VS_STD_2D_OUTPUT) 0.f;
    
    float4 vWorldPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    float4 vViewPos = mul(vWorldPos, g_matView);
    float4 vProjPos = mul(vViewPos, g_matProj);

    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    return output;
}

VS_STD_2D_OUTPUT VS_Std_2D_Instancing(VS_STD_2D_INPUT _input)
{
    VS_STD_2D_OUTPUT output = (VS_STD_2D_OUTPUT) 0.f;
    
    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_Std_2D(VS_STD_2D_OUTPUT _input)
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
        if(hasTex_0)
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

    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}

//========================
// Standard 3d Shader
// g_tex_0 : Texture
// g_tex_1 : Normal
// g_tex_2 : ShadowMap Tex
//========================

struct VS_STD_3D_INPUT
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

struct VS_STD_3D_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;

    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
};

VS_STD_3D_OUTPUT VS_Std_3D(VS_STD_3D_INPUT _input)
{
    VS_STD_3D_OUTPUT output = (VS_STD_3D_OUTPUT) 0.f;
    
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

VS_STD_3D_OUTPUT VS_Std_3D_Instancing(VS_STD_3D_INPUT _input)
{
    VS_STD_3D_OUTPUT output = (VS_STD_3D_OUTPUT) 0.f;
    
    output.vPos = mul(float4(_input.vPos, 1.f), _input.matWVP);
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_Std_3D(VS_STD_3D_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = (float4) 0.f;
    float4 vTexCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    float3 vNormal = _input.vViewNormal;

    if (hasTex_1)
    {
        vNormal = g_tex_1.Sample(g_default_sam, _input.vUV).xyz;
        vNormal = vNormal * 2.f - 1.f; // 0.f ~ 1.f ---> -1.f ~ 1.f

        float3x3 matTBN = float3x3(_input.vViewTangent, _input.vViewBinormal, _input.vViewNormal);
        vNormal.xyz = mul(vNormal, matTBN);
    }
        
    for (uint i = 0; i < g_iLightCount; ++i)
    {
        tLightInfo info = CalLight(_input.vViewPos, vNormal, g_LightInfo[i]);

          // 그림자 테스트
        float3 vWorldPos = mul(float4(_input.vViewPos, 1.f), g_matInvView).xyz;

        float4 vShadowUV = mul(float4(vWorldPos, 1.f), g_LightInfo[g_int_0].matVP);
        vShadowUV.xy /= vShadowUV.w; // 투영좌표를 UV 좌표로 변환    
        vShadowUV.x = vShadowUV.x * 0.5f + 0.5f;
        vShadowUV.y = -vShadowUV.y * 0.5f + 0.5f;

         // 광원 시점에서 벋어나면
        if (vShadowUV.x < 0.f || 1.f < vShadowUV.x ||
        vShadowUV.y < 0.f || 1.f < vShadowUV.y)
        {
            vCol.rgb += vTexCol.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
        }
        else
        {
            float fDepth = g_tex_3.Sample(g_default_sam, vShadowUV.xy);
            if (fDepth * vShadowUV.w <= vShadowUV.z - 0.0001f)
            {
                // 그림자가 생긴다.
                vCol.rgb += vTexCol.rgb * ((info.tCol.vDiff.rgb * 0.3f) + info.tCol.vAmb.rgb);
            }
            else
            {
                vCol.rgb += vTexCol.rgb * (info.tCol.vDiff.rgb + info.tCol.vAmb.rgb) + info.tCol.vSpec.rgb;
            }
        }
    }

    vCol.a = vTexCol.a;
    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}

//=======================
// SkyBox Shader
// g_tex_0 : Sky Texture
//=======================
VS_TEX_OUTPUT VS_Sky(VS_TEX_INPUT _input)
{
    VS_TEX_OUTPUT output = (VS_TEX_OUTPUT) 0.f;
     
    float4 vViewjPos = mul(float4(_input.vPos, 0.f), g_matView);
    float4 vProjPos = mul(vViewjPos, g_matProj);
    
    vProjPos.z = vProjPos.w;
    
    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    return output;
}

VS_TEX_OUTPUT VS_Sky_Instancing(VS_TEX_INPUT _input)
{
    VS_TEX_OUTPUT output = (VS_TEX_OUTPUT) 0.f;
    
    float4 vProjPos = mul(float4(_input.vPos, 1.f), _input.matWVP);

    vProjPos.z = vProjPos.w;

    output.vPos = vProjPos;
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_Sky(VS_TEX_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    output.vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    
    output.vCol_1 = output.vCol;
    output.vCol_2 = output.vCol;

    return output;
}

struct VS_SKYCUBE_OUTPUT
{
    float4 vPos : SV_POSITION;
    float3 vTexCoord : TEXCOORD;
};

VS_SKYCUBE_OUTPUT VS_Sky_Cube(VS_TEX_INPUT _input)
{
    VS_SKYCUBE_OUTPUT output = (VS_SKYCUBE_OUTPUT) 0.f;
     
    float4 vViewjPos = mul(float4(_input.vPos, 0.f), g_matView);
    float4 vProjPos = mul(vViewjPos, g_matProj);
      
    vProjPos.z = vProjPos.w;
    
    output.vPos = vProjPos;
    output.vTexCoord = normalize(_input.vPos);

    return output;
}

PS_OUTPUT PS_Sky_Cube(VS_SKYCUBE_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    if (hasTexArr_7)
        output.vCol = g_texarr_7.Sample(g_default_sam, _input.vTexCoord);
    else
        output.vCol = float4(1.f, 0.f, 0.f, 1.f);
    
   output.vCol.xyz *= 0.7f;

    output.vCol_1 = output.vCol;
    output.vCol_2 = output.vCol;

    return output;
}

//=======================
// HP Shader
// g_tex_0 : HP Tex
// g_float_0 : Present
//=======================

PS_OUTPUT PS_HP(VS_TEX_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = float4(1.f, 0.f, 0.f, 1.f);

    //_input.vUV.x *= _input.vUV.x / g_float_0;

    if(hasTex_0)
        vCol = g_tex_0.Sample(g_default_sam, _input.vUV);

    float fAlpha = vCol.a;
    if (_input.vUV.x > g_float_0)
        fAlpha = 0.f;

    vCol.a = fAlpha;
   
    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}


#endif