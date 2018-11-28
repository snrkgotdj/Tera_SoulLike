#ifndef HLSL_GEOMETRY
#define HLSL_GEOMETRY

#include "value.fx"

struct PS_OUTPUT
{
    float4 vCol : SV_Target;
    float4 vCol_1 : SV_Target1;
    float4 vCol_2 : SV_Target2;
};

struct VS_INPUT
{
    float3 vPos : POSITION;
};

struct VS_OUTPUT
{
    float4 vPos : SV_Position;
};

VS_OUTPUT VS_TestGeo(VS_INPUT _input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0.f;

    output.vPos = mul(float4(_input.vPos, 1.f), g_matWVP);

    return output;
}

struct GS_OUTPUT
{
    float4 vPos : SV_POSITION;
};


//[maxvertexcount(8)]
//void GS_TestGeo(triangle VS_OUTPUT _input[3], inout TriangleStream<GS_OUTPUT> _output)
//{
//    GS_OUTPUT output = (GS_OUTPUT) 0.f;

//    output.vPos = _input[0].vPos;
//    _output.Append(output);
//    output.vPos = _input[1].vPos;
//    _output.Append(output);
//    output.vPos = _input[2].vPos;
//    _output.Append(output);
    
//    //_output.RestartStrip();
    
//    //output.vPos = _input[0].vPos;
//    //output.vPos.y -= 1;
//    //_output.Append(output);
//    //output.vPos = _input[2].vPos;
//    //output.vPos.y -= 1;
//    //_output.Append(output);

//    float4 vPos = mul(float4(0.f, -0.5f, 0.f, 1.f), g_matWVP);
//    output.vPos = vPos;
//    output.vPos.y -= 1;
//    _output.Append(output);

//    vPos = mul(float4(0.5f, -0.5f, 0.f, 1.f), g_matWVP);

//    _output.RestartStrip();

//    //float4 vPos = mul(float4(-0.5f, 0.5f, 0.f, 1.f), g_matWVP);
//    //output.vPos = vPos;
//    //_output.Append(output);

//    //vPos = mul(float4(0.5f, 0.5f, 0.f, 1.f), g_matWVP);
//    //output.vPos = vPos;
//    //_output.Append(output);

//    //vPos = mul(float4(-0.5f, -0.5f, 0.f, 1.f), g_matWVP);
//    //output.vPos = vPos;
//    //_output.Append(output);

//}

[maxvertexcount(4)]
void GS_TestGeo(point VS_OUTPUT _input[1], inout TriangleStream<GS_OUTPUT> _output)
{
    GS_OUTPUT output = (GS_OUTPUT) 0.f;

    float4 vPos = _input[0].vPos;
    output.vPos = vPos;
    _output.Append(output);

    vPos = _input[0].vPos;
    vPos.x += 10;
    output.vPos = vPos;
    _output.Append(output);

    vPos = _input[0].vPos;
    vPos.y -= 10;
    output.vPos = vPos;
    _output.Append(output);

    vPos = _input[0].vPos;
    vPos.x += 10;
    vPos.y -= 10;
    output.vPos = vPos;
    _output.Append(output);

    _output.RestartStrip();



}

PS_OUTPUT PS_TestGeo(GS_OUTPUT _input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0.f;

    float4 vCol = float4(1.f, 0.f, 1.f, 1.f);

    output.vCol = vCol;
    output.vCol_1 = vCol;
    output.vCol_2 = vCol;

    return output;
}


#endif