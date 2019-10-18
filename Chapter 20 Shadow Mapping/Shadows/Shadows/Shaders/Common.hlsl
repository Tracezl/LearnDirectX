#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

struct MaterialData
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MatTransform;
    uint DiffuseMapIndex;
    uint NormalMapIndex;
    uint MatPad1;
    uint MatPad2;
};

TextureCube gCubeMap : register(t0);
Texture2D gShadowMap : register(t1);

Texture2D gTextureMaps[10] : register(t2);

StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);
SamplerComparisonState gsamShadow : register(s6); // �Ƚϲ�����,CPU�����ñȽϺ���Ϊ <=

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex;
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float4x4 gShadowTransform; // �����������ռ�任������ռ�
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;
    Light gLights[MaxLights];
};

// ������ͼ�����任������ռ�
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW)
{
	// ��ѹ��[0,1] -> [-1,1]
    float3 normalT = 2.0f * normalMapSample - 1.0f;

	// �����淶��������
    float3 N = uintNormalW; // ����ռ��еķ���
    float3 T = normalize(tangentW - dot(tangentW, N) * N); // ����ռ��е�����
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// ������ռ�任������ռ�
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

// PCF: Percentage Closer Filtering, �ٷֱȽ�������
// ��Ӱ����[0,1],����һ����,0��ʾλ����Ӱ֮��,1��ʾ��Ӱ֮��,0~1��ʾ���ִ�����Ӱ֮��
float CalcShadowFactor(float4 shadowPosH)
{
	// ͨ������w�����ͶӰ����
    shadowPosH.xyz /= shadowPosH.w;

	// ��NDC�ռ��е����ֵ
    float depth = shadowPosH.z;

    uint width, height, numMips;
    gShadowMap.GetDimensions(0, width, height, numMips); // ��ȡ��ͼ�ߴ���Ϣ,�������

	// ���ش�С
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
	// �������Ϊ���ĵ�9����
    const float2 offsets[9] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

	[unroll]
    for (int i = 0; i < 9; ++i)
    {
		// ʹ�ñȽϲ�����,LevelZero��ζ��ֻ������ߵ�mipmap�㼶�в���ִ�д˺���
        percentLit += gShadowMap.SampleCmpLevelZero(gsamShadow,
			shadowPosH.xy + offsets[i], depth).r;
    }

    return percentLit / 9.0f;
}