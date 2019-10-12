#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtil.hlsl"

TextureCube gCubeMap : register(t0);
Texture2D gTextureMaps[10] : register(t1);

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex; // ָ���˲��ʽṹ�建���е�����
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};

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
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
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

// ��һ������ͼ���������߿ռ�任������ռ�,������Ѿ�������ռ��еķ�����������(VS�д����)
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW)
{
	// ���������淶��
    float3 N = uintNormalW; // ����
    float3 T = normalize(tangentW - dot(tangentW, N) * N); // ����
    float3 B = cross(N, T); // ������
	// ���߿ռ� -> ����ռ� �任����
    float3x3 TBN = float3x3(T, B, N);
	
	// ��������ѹ��: ��λ������ÿ��Ԫ�ض���[-1,1]������,����任��[0,1],��*255�õ�RGBֵ
	// �������Ľ�ѹ������: ��ÿ����������ɷ�Χ[0,1]��ѹ��[-1,1]����
    float3 normalT = 2.0f * normalMapSample - 1.0f;

	// �����ߴ����߿ռ�任������ռ�
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}