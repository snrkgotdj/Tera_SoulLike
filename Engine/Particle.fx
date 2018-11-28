#ifndef HLSL_PARTICLE
#define HLSL_PARTICLE

#include "value.fx"
#include "func.fx"

#define PT_EMITTER 0

//=================
// Stream Out Shader 
// g_float_0 : DeltaTime
// g_vec4_0 : Random
// g_vec4_1 : EmitPos
//=================
struct Particle
{
    float3 vPos : POSITION;
    float2 vSize : SIZE;
    float3 vDir : VELOCITY;
    float fAge : AGE;
    uint iType : TYPE;
};

Particle VS_StreamOut(Particle _input)
{
    return _input;
}

[maxvertexcount(2)]
void GS_StreamOut(point Particle _input[1], inout PointStream<Particle> _stream)
{
    _stream.Append(_input[0]);

    //_input[0].fAge += g_float_0;

    //if (_input[0].iType == PT_EMITTER)
    //{
    //    if (_input[0].fAge > 0.1f)
    //    {
    //        float3 vRandom = g_vec4_0.xyz;

    //        Particle p = (Particle) 0;
    //        p.vPos = g_vec4_1.xyz;
    //        p.vSize = _input[0].vSize;
    //        p.vDir = vRandom;
    //        p.fAge = 0.f;

    //        _stream.Append(p);
    //        _input[0].fAge = 0;
    //    }
    //    _stream.Append(_input[0]);
    //}

    //else
    //{
    //    // 1초가 넘으면 파괴
    //    if(_input[0].fAge <= 1.f)
    //        _stream.Append(_input[0]);
    //}
    
}

//=================
// DrawParticle Out Shader 
// g_vec4_0 : vCamAxisX
// g_vec4_1 : vCamAxisY
//=================

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vSize : SIZE;
    float iType : TYPE;
};

VS_OUT VS_ParticleDraw(Particle _input)
{
    VS_OUT output = (VS_OUT) 0.f;

    output.vPos = _input.vPos;
    output.vSize = _input.vSize;
    output.iType = _input.iType;

    return output;
}

struct GS_OUT
{
    float4 vPos : SV_Position;
    float2 vUV : TEXCOORD;
};

[maxvertexcount(4)]
void GS_ParticleDraw(point VS_OUT _input[1], inout TriangleStream<GS_OUT> _triStream)
{
    float4 temp = mul(float4(_input[0].vPos, 1.f), g_matWorld);
    _input[0].vPos = temp.xyz;

    float fHalfWidth = 0.5 * _input[0].vSize.x;
    float fHalfHeight = 0.5 * _input[0].vSize.y;

    float4 v[4] =
    {
        float4(0, 0, 0, 0),
        float4(0, 0, 0, 0),
        float4(0, 0, 0, 0),
        float4(0, 0, 0, 0)
    };

    v[0] = float4(_input[0].vPos + fHalfWidth * g_vec4_0.xyz - fHalfHeight * g_vec4_1.xyz, 1);
    v[1] = float4(_input[0].vPos + fHalfWidth * g_vec4_0.xyz + fHalfHeight * g_vec4_1.xyz, 1);
    v[2] = float4(_input[0].vPos - fHalfWidth * g_vec4_0.xyz - fHalfHeight * g_vec4_1.xyz, 1);
    v[3] = float4(_input[0].vPos - fHalfWidth * g_vec4_0.xyz + fHalfHeight * g_vec4_1.xyz, 1);

    GS_OUT output = (GS_OUT) 0.f;
        
    float2 vUV[4] =
    {
        float2(0.f, 1.f),
		    float2(0.f, 0.f),
		    float2(1.f, 1.f),
		    float2(1.f, 0.f)
    };

    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        output.vPos = mul(v[i], g_matView);
        output.vPos = mul(output.vPos, g_matProj);
        output.vUV = vUV[i];
        _triStream.Append(output);
    }

    //GS_OUT output = (GS_OUT) 0.f;

    //float fHalfWidth = 0.5 * _input[0].vSize.x;
    //float fHalfHeight = 0.5 * _input[0].vSize.y;
    //float fDist = 0;
    //if (_input[0].vSize.x == 10)
    //    fDist = 20;
    //else
    //    fDist = 10;

    //float3 vPos = _input[0].vPos;
    //output.vPos = mul(mul(float4(vPos, 1.f), g_matView), g_matProj);;
    //_triStream.Append(output);

    //vPos = _input[0].vPos;
    //vPos.x += 10;
    //output.vPos = mul(mul(float4(vPos, 1.f), g_matView), g_matProj);;
    //_triStream.Append(output);

    //vPos = _input[0].vPos;
    //vPos.y -= fDist;
    //output.vPos = mul(mul(float4(vPos, 1.f), g_matView), g_matProj);;
    //_triStream.Append(output);

    //vPos = _input[0].vPos;
    //vPos.x += 10;
    //vPos.y -= fDist;
    //output.vPos = mul(mul(float4(vPos, 1.f), g_matView), g_matProj);;
    //_triStream.Append(output);

    //_triStream.RestartStrip();

}

struct PS_OUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};

PS_OUT PS_ParticleDraw(GS_OUT _input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0.f;

    float4 vCol = float4(1.f, 0.f, 0.f, 1.f);

    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}

#endif


//float4 temp = mul(float4(_input[0].vPos, 1.f), g_matWorld);
//    _input[0].vPos = temp.
//xyz;

//float fHalfWidth = 0.5 * _input[0].vSize.x;
//float fHalfHeight = 0.5 * _input[0].vSize.y;

//float4 v[4] =
//{
//    float4(0, 0, 0, 0),
//        float4(0, 0, 0, 0),
//        float4(0, 0, 0, 0),
//        float4(0, 0, 0, 0)
//};

//    v[0] = float4(_input[0].vPos + fHalfWidth * g_vec4_0.xyz - fHalfHeight * g_vec4_1.xyz, 1);
//    v[0] = float4(_input[0].vPos + fHalfWidth * g_vec4_0.xyz + fHalfHeight * g_vec4_1.xyz, 1);
//    v[0] = float4(_input[0].vPos - fHalfWidth * g_vec4_0.xyz - fHalfHeight * g_vec4_1.xyz, 1);
//    v[0] = float4(_input[0].vPos - fHalfWidth * g_vec4_0.xyz + fHalfHeight * g_vec4_1.xyz, 1);

//GS_OUT output = (GS_OUT) 0.f;
        
//float2 vUV[4] =
//{
//    float2(0.f, 1.f),
//		    float2(0.f, 0.f),
//		    float2(1.f, 1.f),
//		    float2(1.f, 0.f)
//};

//    [unroll]
//    for (
//int i = 0;i < 4; ++i)
//    {
//        output.vPos = mul(v[i], g_matView);
//        output.vPos = mul(output.vPos, g_matProj);
//        output.vUV =
//vUV[ i];
//        _triStream.Append(output);
//    }