// ��Դ������Ĭ��ֵ
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// �����˹������õĽṹ���뺯��
#include "LightingUtil.hlsl"

Texture2D    gDiffuseMap : register(t0); // ����
SamplerState gsamLinear  : register(s0); // ������

// ÿ֡���仯�ĳ�������
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform; // UV��Ҫ���������
};

// ���ƹ��������õĳ�������
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

	// [0,NUM_DIR_LIGHTS]:�����Դ
	// [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS]:���Դ ...
	Light gLights[MaxLights];
};

// ÿ�ֲ��ʲ�ͬ�ĳ�������
cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float  gRoughness;
	float4x4 gMatTransform; // UV��Ҫ���������
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION; // ��βü��ռ�
	float3 PosW    : POSITION; // ��������
	float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
};

// ������Ķ���ֲ�����ϵλ�úͷ���ת��Ϊ��������ϵ
// ������������λ��ת��Ϊ��βü��ռ�
VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Transform to world space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// ����������е��ǵȱ�����,������Ҫʹ������������ת�þ���
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	// ������任����βü��ռ�
	vout.PosH = mul(posW, gViewProj);

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, gMatTransform).xy;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// ����������ȡ�����ص������䷴����
	// �����������볣���������еķ��������
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamLinear, pin.TexC) * gDiffuseAlbedo;

	// �Է��߽��в�ֵ���ܵ�����ǹ淶��,�����Ҫ�ٴζ������й淶������
	pin.NormalW = normalize(pin.NormalW);

	// ���浽�۲������
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ��ӹ���
	float4 ambient = gAmbientLight * diffuseAlbedo;

	// ֱ�ӹ���
	const float shininess = 1.0f - gRoughness;
	Material mat = { diffuseAlbedo, gFresnelR0, shininess };
	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		pin.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambient + directLight;

	// ������������л�ȡalpha�ĳ����ֶ�
	litColor.a = diffuseAlbedo.a;

	return litColor;
}
