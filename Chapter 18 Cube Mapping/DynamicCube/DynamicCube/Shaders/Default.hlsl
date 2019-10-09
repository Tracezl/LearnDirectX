#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};

// ���� VertexIn ���� VertexOut, ��Ϊ������ɫ�����������
VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    MaterialData matData = gMaterialData[gMaterialIndex];
	
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW;
	
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
	
    vout.PosH = mul(posW, gViewProj);
	
    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

// 1,������ = gAmbientLight * diffuseAlbedo(��ͼ & UV ����)
// 2,��Դ = ComputeLighting()
// 3,��պ���ɫ�ӳ� = shininess * fresnelFactor(����ϵ��-ʯ���) * reflectionColor(������ͼ & ������������)
float4 PS(VertexOut pin) : SV_Target
{
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    uint diffuseTexIndex = matData.DiffuseMapIndex;

	// �����䷴����,ͨ����ͼ uv �������,Ӱ�컷����
    diffuseAlbedo *= gDiffuseMap[diffuseTexIndex].Sample(gsamLinearWrap, pin.TexC);
	
    pin.NormalW = normalize(pin.NormalW);

	// ���ص㵽���������
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ���������ɫ��Ӱ��
    float4 ambient = gAmbientLight * diffuseAlbedo;

    const float shininess = 1.0f - roughness;
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float3 shadowFactor = 1.0f;

	// ��Դ����ɫ��Ӱ��
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
	pin.NormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// ��������
    float3 r = reflect(-toEyeW, pin.NormalW);
	// ��պе���ɫӰ��,ͨ����������ͼ �������� �������
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, pin.NormalW, r);
	// ��պв�����ɫ��Ҫ * ����ϵ��(ʯ���) * ����ϵ��
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    litColor.a = diffuseAlbedo.a;

    return litColor;
}
