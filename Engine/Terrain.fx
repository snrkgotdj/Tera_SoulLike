#ifndef _HLSL_TERRAIN
#define _HLSL_TERRAIN

#include "value.fx"
#include "func.fx"

//================================
// Terrain Shader 
// g_float_0 : X Face Count
// g_float_1 : Z Face Count
// g_vec4_0 : (x, y) Brush Position Ratio, (z, w) Brush Radius Ratio
// g_vec4_1 : (x, y) World Distance of Texel, z World Y scale
// g_vec4_2 : Main Cam Pos

// g_texarr_0 : Diffuse Tex Arr
// g_texarr_1 : Normal Tex Arr
// g_tex_0 : Height Map
// g_tex_1 : Weight Tex
// g_tex_2 : Brush Tex
//===============================

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

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    float4 vColor : COLOR;

    row_major matrix matWorld : WORLD;
    row_major matrix matWV : WV;
    row_major matrix matWVP : WVP;

    uint iInstanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 vColor : COLOR;

    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
    float2 vWeightUV : TEXCOORD1;
    float2 vBrushUV : TEXCOORD2;
};

VS_OUTPUT VS_Terrain(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;

    output.vWeightUV = float2(_input.vUV.x / g_float_0, _input.vUV.y / g_float_1);

    float2 vBrushPos = float2(g_vec4_0.x, 1.f - g_vec4_0.y) * float2(g_float_0, g_float_1);
    float2 vBrushRadius = g_vec4_0.zw * float2(g_float_0, g_float_1);

    output.vBrushUV.x = 0.5f + (_input.vPos.x - vBrushPos.x) / (vBrushRadius.x * 2);
    output.vBrushUV.y = 0.5f - (_input.vPos.z - vBrushPos.y) / (vBrushRadius.y * 2);

    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vColor = _input.vColor;
    output.vUV = _input.vUV;

    return output;
}

struct PatchTessFactor
{
    float arrEdge[3] : SV_TessFactor;
    float fInsideFactor : SV_InsideTessFactor;
};

PatchTessFactor ConstantHS(InputPatch<VS_OUTPUT, 3> _patch, uint _patchID : SV_PrimitiveID)
{
    PatchTessFactor output = (PatchTessFactor) 0.f;

    if(0 == g_int_0)
    {
        output.arrEdge[0] = 1;
        output.arrEdge[1] = 1;
        output.arrEdge[2] = 1;
        output.fInsideFactor = 0;

        return output;
    }

    float3 vStdPos = (_patch[0].vPos.xyz + _patch[1].vPos.xyz) / 2.f;
    float fXStep = _patch[2].vPos.x - _patch[1].vPos.x;
    float fZStep = _patch[2].vPos.z - _patch[0].vPos.z;

    float3 vXPos = float3(vStdPos.x + fXStep, vStdPos.y, vStdPos.z);
    float3 vZPos = float3(vStdPos.x, vStdPos.y, vStdPos.z + fZStep);
    
    float fTess = CalTessLvl(500.f, 5000.f, 4.f, 0.f, vStdPos);
    float fXTess = CalTessLvl(500.f, 5000.f, 4.f, 0.f, vXPos);
    float fZTess = CalTessLvl(500.f, 5000.f, 4.f, 0.f, vZPos);

    if (fTess < fZTess)
        output.arrEdge[0] = fZTess;
    else
        output.arrEdge[0] = fTess;

    if (fTess < fXTess)
        output.arrEdge[1] = fXTess;
    else
        output.arrEdge[1] = fTess;
    
    output.arrEdge[2] = fTess;
    output.fInsideFactor = fTess;

    return output;
}

struct HULL_OUTPUT
{
    float3 vPos : POSITION;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;

    float2 vWeightUV : TEXCOORD1;
    float2 vBrushUV : TEXCOORD2;
};

[domain("tri")]
[partitioning("integer")] // [partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[maxtessfactor(64.f)]
[patchconstantfunc("ConstantHS")]
HULL_OUTPUT HS(InputPatch<VS_OUTPUT, 3> _patch, uint i : SV_OutputControlPointID
            , uint _patchID : SV_PrimitiveID)
{
    HULL_OUTPUT output = (HULL_OUTPUT) 0.f;

    output.vPos = _patch[i].vPos.xyz;
    output.vColor = _patch[i].vColor;
    output.vUV = _patch[i].vUV;

    output.vWeightUV = _patch[i].vWeightUV;
    output.vBrushUV = _patch[i].vBrushUV;
       
    return output;
}

struct DOMAIN_OUTPUT
{
    float4 vPos : SV_Position;
    float4 vColor : COLOR;
    float2 vUV : TEXCOORD;

    float2 vWeightUV : TEXCOORD1;
    float2 vBrushUV : TEXCOORD2;

    float3 vWorldPos : POSITION;
    float3 vViewPos : POSITION1;
    float3 vProjPos : POSITION2;
};

[domain("tri")]
DOMAIN_OUTPUT DS(PatchTessFactor _patchFactor, float3 _vUV : SV_DomainLocation
                , const OutputPatch<HULL_OUTPUT, 3> _ControlPoints)
{
    DOMAIN_OUTPUT output = (DOMAIN_OUTPUT) 0.f;


    float2 vWeightUV = _vUV.x * _ControlPoints[0].vWeightUV + _vUV.y * _ControlPoints[1].vWeightUV + _vUV.z * _ControlPoints[2].vWeightUV;

    float fHeight = 0.f;
    // 높이값 추출 후 셋팅
    if (hasTex_0)
    {
        fHeight = g_tex_0.SampleLevel(g_default_sam, vWeightUV, 0.f);
        fHeight *= g_vec4_1.z;
    }

    output.vWorldPos = _vUV.x * _ControlPoints[0].vPos + _vUV.y * _ControlPoints[1].vPos + _vUV.z * _ControlPoints[2].vPos;
    output.vWorldPos.y = fHeight;
    
    output.vViewPos = mul(float4(output.vWorldPos, 1.f), g_matView).xyz;
    output.vPos = mul(float4(output.vViewPos, 1.f), g_matProj);
    output.vProjPos = output.vPos.xyz;
    output.vUV = _vUV.x * _ControlPoints[0].vUV + _vUV.y * _ControlPoints[1].vUV + _vUV.z * _ControlPoints[2].vUV;

    output.vBrushUV = _vUV.x * _ControlPoints[0].vBrushUV + _vUV.y * _ControlPoints[1].vBrushUV + _vUV.z * _ControlPoints[2].vBrushUV;
    output.vWeightUV = _vUV.x * _ControlPoints[0].vWeightUV + _vUV.y * _ControlPoints[1].vWeightUV + _vUV.z * _ControlPoints[2].vWeightUV;;

    output.vColor = _vUV.x * _ControlPoints[0].vColor + _vUV.y * _ControlPoints[1].vColor + _vUV.z * _ControlPoints[2].vColor;
    return output;
}

#define NAVI_MOD 3

PS_OUTPUT PS_Terrain(DOMAIN_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    // Diffuse
    float4 vCol = float4(0.f, 0.f, 0.f, 1.f);
    float4 vWeight = g_tex_1.Sample(g_default_sam, _input.vWeightUV);
    if (g_int_1 == NAVI_MOD)
    {
        if(_input.vColor.r == 0.f)
            vCol = float4(0.f, 1.f, 0.f, 0.f);
        else
        {
            vCol = float4(1.f, 1.f, 1.f, 0.f);
          
        }
    }
    else
    {
        if (hasTexArr_0)
        {
            for (int i = 0; i < 4; ++i)
            {
                vCol += vWeight[i] * g_texarr_0.Sample(g_default_sam, float3(_input.vUV.x, _input.vUV.y, (float) i));
            }
        }

        if (0.f < _input.vBrushUV.x && _input.vBrushUV.x < 1.f
        && 0.f < _input.vBrushUV.y && _input.vBrushUV.y < 1.f)
        {
            output.vDiffuse.a = g_tex_2.Sample(g_default_sam, _input.vBrushUV).a;
        }
        else
        {
            output.vDiffuse.a = 0.f;
        }
    }
    
    output.vDiffuse.rgb = vCol.rgb;

      // Normal
    float3 vViewNormal = (float3) 0.f;
    
    float fWidth = 0.f;
    float fHeight = 0.f;
    g_tex_0.GetDimensions(fWidth, fHeight);

    float fUVStepX = 8.f / fWidth;
    float fUVStepZ = 8.f / fHeight;

    //normal, tangent, binormal
    // 재계산

    float3 vRightPos = float3(_input.vWorldPos.x + g_vec4_1.x
                             , g_tex_0.Sample(g_default_sam, float2(_input.vWeightUV.x + fUVStepX, _input.vWeightUV.y)).x * g_vec4_1.z
                             , _input.vWorldPos.z);

    float3 vTopPos = float3(_input.vWorldPos.x
                          , g_tex_0.Sample(g_default_sam, float2(_input.vWeightUV.x, _input.vWeightUV.y - fUVStepZ)).x * g_vec4_1.z
                          , _input.vWorldPos.z + g_vec4_1.y);
              
    // world 기준
    float3 vBinormal = normalize(vTopPos - _input.vWorldPos);
    float3 vTangent = normalize(vRightPos - _input.vWorldPos);
    float3 vNormal = normalize(cross(vBinormal, vTangent));

    if (hasTexArr_1)
    {
        float fMaxWeight = vWeight[0];
        int iMaxIdx = 0;
        for (int i = 1; i < 4; ++i)
        {
            if (fMaxWeight < vWeight[i])
            {
                iMaxIdx = i;
                fMaxWeight = vWeight[i];
            }
        }
      
        vViewNormal = g_texarr_1.Sample(g_default_sam, float3(_input.vUV.x, _input.vUV.y, iMaxIdx)).xyz;
        vViewNormal = vViewNormal * 2.f - 1.f; // 0.f ~ 1.f  --->  -1.f ~ 1.f
        vViewNormal = normalize(vViewNormal);

        float3x3 matTBN = float3x3(vTangent, vBinormal, vNormal);
        vViewNormal.xyz = mul(vViewNormal.xyz, matTBN);
        vViewNormal.xyz = normalize(mul(float4(vViewNormal, 0.f), g_matView)).xyz;
    }
    else
    {
        vViewNormal = normalize(mul(float4(0.f, 1.f, 0.f, 0.f), g_matView).xyz);
    }
    
    output.vNormal.xyz = vViewNormal;

    // Position
    output.vPosition.xyz = _input.vViewPos;

    // Depth
    output.vDepth.x = _input.vProjPos.z;

     // Brush
    
    output.vSpecular = (float4) 0.f;
    output.vEmissive = (float4) 0.f;
    output.vVelocity = (float4) 0.f;

    return output;
}

#endif