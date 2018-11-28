
#ifndef _HLSL_VALUE
#define _HLSL_VALUE


// 구조체
struct tLightCol
{
    float4 vDiff; // 빛의 색상
    float4 vSpec; // 주변광 
    float4 vAmb; // 반사광
};

struct tLightInfo
{
    tLightCol   tCol;
    float4      vLightPos; // 광원 위치
    float3      vLightDir;
    uint        iLightType;
    float2      vSpotAngle;     // x: 외부각, y: 내부각
    float2      vPad;
    matrix      matVP;
};


// 레지스터 변수 선언
cbuffer TransformBuffer : register(b0)
{
    matrix g_matWorld;
    matrix g_matView;
    matrix g_matProj;
    matrix g_matWV;
    matrix g_matWVP;
    matrix g_matInvView;
    matrix g_matInvProj;
    matrix g_matBeforeWVP;
    float4 g_fDelta;        // x : DT, y : 누적 DT z : Rim Num, w : 스카이 박스 밝기
}

cbuffer INT : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    int g_int_4;
    int g_int_5;
    int g_int_6;
    int g_int_7;
}

cbuffer FLOAT : register(b2)
{
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    float g_float_4;
    float g_float_5;
    float g_float_6;
    float g_float_7;
}

cbuffer VEC4 : register(b3)
{
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    float4 g_vec4_4;
    float4 g_vec4_5;
    float4 g_vec4_6;
    float4 g_vec4_7;
}

cbuffer LightArr : register(b4)
{
    tLightInfo g_LightInfo[5];
    uint g_iLightCount;
    float3 vPad;
}

cbuffer TexCheck : register(b5)
{
    uint hasTex_0;
    uint hasTex_1;
    uint hasTex_2;
    uint hasTex_3;
    uint hasTex_4;
    uint hasTex_5;
    uint hasTex_6;
    uint hasTex_7;
    uint hasTexArr_0;
    uint hasTexArr_1;
    uint hasTexArr_2;
    uint hasTexArr_3;
    uint hasTexArr_4;
    uint hasTexArr_5;
    uint hasTexArr_6;
    uint hasTexArr_7;
}

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);
Texture2D g_tex_6 : register(t6);
Texture2D g_tex_7 : register(t7);

Texture2DArray g_texarr_0 : register(t8);
Texture2DArray g_texarr_1 : register(t9);
Texture2DArray g_texarr_2 : register(t10);
Texture2DArray g_texarr_3 : register(t11);
Texture2DArray g_texarr_4 : register(t12);
Texture2DArray g_texarr_5 : register(t13);
Texture2DArray g_texarr_6 : register(t14);
TextureCube g_texarr_7 : register(t15);

SamplerState g_default_sam : register(s0);
SamplerState g_Linear_sam : register(s1);

// Animation
cbuffer VEC4 : register(b13)
{
    float4 g_vec4_uv; // xy(uv) z(width) w(height)
    float4 g_vec4_anim; // x(Animatior 유무)
}

Texture2D g_tex_anim : register(t16);
Texture2D g_tex_bone : register(t17);
Texture2D g_tex_bone_prev : register(t18);

// 전역변수
static float g_vGaussian[9] =
{
    1.0f, 2.0f, 1.0f,
	2.0f, 4.0f, 2.0f,
	1.0f, 2.0f, 1.0f
};

static float g_vGaussian2[25] =
{
    1.0f, 2.0f, 4.0f,   2.0f, 1.0f,     // 10
	2.0f, 4.0f, 8.0f,   4.0f, 2.0f,     // 20
    4.0f, 8.0f, 16.0f,  8.0f, 4.0f,     // 40
    2.0f, 4.0f, 8.0f,   4.0f, 2.0f,     // 20
    1.0f, 2.0f, 4.0f,   2.0f, 1.0f,     // 10
};


cbuffer cbFixed
{
    //입자의 가속을 위한 알짜 상수 가속도.
    float3 g_AccelW = { 0.f, 7.8f, 0.0f };
    float2 g_QuadTexC[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };
};

#endif