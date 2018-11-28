#ifndef _HLSL_FILTER
#define _HLSL_FILTER

#include "value.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
};

struct VS_INPUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 vPos : SV_POSITION;
    float2 vUV : TEXCOORD;
};

//===========================
// Gaussian Filter Shader
// g_tex_0 : Texture
// g_float_0 : Texcell StepU
// g_float_1 : Texcell StepV
//===========================
VS_OUTPUT VS_Gaussian(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_Gaussian(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    
    float stepU = 1 / 1600.f;
    float stepV = 1 / 900.f;
    
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            float2 vUV = _input.vUV + float2((i - 1) * stepU, (j - 1) * stepV);
            output.vCol += g_tex_0.Sample(g_default_sam, vUV) * g_vGaussian2[i * 5 + j];
        }
    }
       
    output.vCol /= 100.0f;
        
    return output;
}

float2x2 RotationMatrix(float rotation)
{
    float c = cos(rotation);
    float s = sin(rotation);

    return float2x2(c, -s, s, c);
}


VS_OUTPUT VS_Distortion(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;
    return output;
}

PS_OUTPUT PS_Distortion(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;
    // lens distortion coefficient (between
    float k = -1.f;
       
    // cubic distortion value
    float kcube = 0.0;
       
    float r2 = (_input.vUV.x - 0.5) * (_input.vUV.x - 0.5) + (_input.vUV.y - 0.5) * (_input.vUV.y - 0.5);
    float f = 0;
       
    //only compute the cubic distortion if necessary
    if (kcube == 0.0)
    {
        f = 1 + r2 * k;
    }
    else
    {
        f = 1 + r2 * (k + kcube * sqrt(r2));
    };
       
    // get the right pixel for the current position
    float x = f * (_input.vUV.x - 0.5) + 0.5;
    float y = f * (_input.vUV.y - 0.5) + 0.5;
    float4 inputDistord = g_tex_0.Sample(g_default_sam, float2(x, y));
 
    output.vCol = inputDistord;
    output.vCol.a = 1.f;

    return output;
}

//===========================
// Detort Filter Shader
// g_tex_0 : Texture
//===========================
VS_OUTPUT VS_Detort(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;

    return output;
}

PS_OUTPUT PS_Detort(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float2 vUV = (float2) 0.f;
    vUV.x = _input.vUV.x;
    vUV.y = _input.vUV.y + sin(_input.vUV.y * 100) * 0.03f;

    output.vCol = g_tex_0.Sample(g_default_sam, vUV);
    
    return output;
}

//===========================
// OldFilm Filter Shader
// g_tex_0 : Texture
// g_float_0 : Texcell StepU
// g_float_1 : Texcell StepV
//===========================
VS_OUTPUT VS_OldFilm(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;
    output.vPos = mul(float4(_input.vPos, 1.f), g_matWorld);
    output.vUV = _input.vUV;

    return output;
}

float2 PixelKernelH[13] =
{
    { -6, 0 },
    { -5, 0 },
    { -4, 0 },
    { -3, 0 },
    { -2, 0 },
    { -1, 0 },
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
    { 4, 0 },
    { 5, 0 },
    { 6, 0 },
};

 

//텍스처의 임의의 위치의 픽셀에 접근하기 위한 좌표 ( 세로 방향)

float2 PixelKernelV[13] =
{
    { 0, -6 },
    { 0, -5 },
    { 0, -4 },
    { 0, -3 },
    { 0, -2 },
    { 0, -1 },
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 0, 4 },
    { 0, 5 },
    { 0, 6 },
};

 

//미리 계산해 둔 가우스 필터의 마스크 값
float BlurWeights[13] =
{
    0.002216, 0.008764, 0.026995, 0.064759, 0.120985, 0.176033, 0.199471, 0.176033, 0.120985, 0.064759,
    0.026995, 0.008764, 0.002216,

};

PS_OUTPUT PS_OldFilm(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    //output.vCol -= g_tex_0.Sample(g_default_sam, _input.vUV.xy - 0.003) * 2.7f;
    //output.vCol += g_tex_0.Sample(g_default_sam, _input.vUV.xy + 0.003) * 2.7f;
    
    //output.vCol = (output.vCol.r + output.vCol.g + output.vCol.b) / 3.f;

    float2 rcpres = { 0.0025, 0.0033333333333333333333333333333333f };

    float4 vCol = g_tex_0.Sample(g_default_sam, _input.vUV);
    vCol = pow(vCol, 32);

    float4 vCol2 = 0.f;
 
    for (int index = 0; index < 13; ++index)
    {
        vCol2 += g_tex_0.Sample(g_default_sam, _input.vUV + (PixelKernelH[index])) * BlurWeights[index];
        vCol2 += g_tex_0.Sample(g_default_sam, _input.vUV + (PixelKernelV[index])) * BlurWeights[index];
    }


    float4 vCol3 = g_tex_0.Sample(g_default_sam, _input.vUV);

    output.vCol = vCol + vCol2 + vCol3;

    return output;
}

//===========================
// UnderWater Filter Shader
// g_tex_0 : Texture
// g_float_0 : Timer
//===========================

PS_OUTPUT PS_UnderWater(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float amp = 0.1f;
    float turb = 10.f;
    float border = 0.9f;
    float2 texCoo = _input.vUV;
    float cooX = _input.vUV.x - 0.5;
    float cooY = _input.vUV.y - 0.5;
    float2 warp = float2(cooX * (border + sin(g_float_0 + cooX * turb) * amp) + 0.5, cooY * (border + cos(g_float_0 + cooY * turb) * amp) + 0.5);

    output.vCol = g_tex_0.Sample(g_default_sam, warp);

    return output;
}

//===========================
// Ripple Filter Shader
// g_tex_0 : Texture
// g_float_0 : Timer
// g_float_1 : Timer2
//===========================

PS_OUTPUT PS_Ripple(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float fDist = distance(_input.vUV, float2(0.5f, 0.5f));

    if (_input.vUV.x <= 0.01 || _input.vUV.y <= 0.01 || _input.vUV.x >= 0.99 || _input.vUV.y >= 0.99 ||
        fDist <= g_float_1)
    {
    }
    else
    {
        float2 tc = _input.vUV;
        float2 p = -1.0 + 2.0 * tc;
        float len = length(p) * 4.0f;
        _input.vUV = tc + (p / len) * cos(len * 5.0f + g_float_0 * 50.f) * 0.03f;
    }

    float stepU = 1 / 1600.f;
    float stepV = 1 / 900.f;
    
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            float2 vUV = _input.vUV + float2((i - 1) * stepU, (j - 1) * stepV);
            output.vCol += g_tex_0.Sample(g_default_sam, vUV) * g_vGaussian2[i * 5 + j];
        }
    }
       
    output.vCol /= 100.0f;


    return output;
}

//===========================
// Fade Shader
// g_tex_0 : Texture
// g_float_0 : Timer
//===========================

PS_OUTPUT PS_FADE(VS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    output.vCol = float4(0.f, 0.f, 0.f, g_float_0);

    return output;
}

#endif