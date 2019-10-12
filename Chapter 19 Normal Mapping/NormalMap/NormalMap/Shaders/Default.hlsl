#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
    float3 TangentU : TANGENT; // ������ͼ������(����ռ�)
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT; // ������ͼ������
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    MaterialData matData = gMaterialData[gMaterialIndex];

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

	// ����ķ��ߺ������ڶ���ռ���
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

	// ����ת������ռ�
    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);

    vout.PosH = mul(posW, gViewProj);

    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;

    pin.NormalW = normalize(pin.NormalW);

	// ���ߺ���ͼͬ����ͬ��uv
    float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamLinearWrap, pin.TexC);
	// ��һ������ͼ���������߿ռ�任������ռ�,������Ѿ�������ռ��еķ�����������(VS�д����)
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	// ȡ��ע���Թرշ�����ͼ
    //bumpedNormalW = pin.NormalW;

    diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamLinearWrap, pin.TexC);

    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ������Ӱ��,������������ͼ��������
    float4 ambient = gAmbientLight * diffuseAlbedo;

	// ������ͼ��alphaͨ���洢�����
    const float shininess = (1.0f - roughness) * normalMapSample.a;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;
	// ֱ���, ���Դ, �۹��Ӱ��, ʹ�ð�͹���߼���
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
		bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// ��պ�Ӱ��
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    litColor.a = diffuseAlbedo.a;

    return litColor;
}