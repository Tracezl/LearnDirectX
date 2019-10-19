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
    float3 TangentU : TANGENT;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 ShadowPosH : POSITION0;
    float3 PosW : POSITION1;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;

    MaterialData matData = gMaterialData[gMaterialIndex];

    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);

    vout.TangentW = mul(vin.TangentU, (float3x3) gWorld);

    vout.PosH = mul(posW, gViewProj);

    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

	// Ϊ������Ӱͼ�����ɵ�ͶӰ��������(����ռ� -> ��Ӱͼ����ռ�)
    vout.ShadowPosH = mul(posW, gShadowTransform);

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

    diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamLinearWrap, pin.TexC);

#ifdef ALPHA_TEST
	clip(diffuseAlbedo.abort-0.1f);
#endif

    pin.NormalW = normalize(pin.NormalW);

    float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamLinearWrap, pin.TexC);
    // ��������ͼ����ת��������ռ�
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	// ȡ��ע���Թرշ�����ͼ
    //bumpedNormalW = pin.NormalW;

	// ���ص㵽�۾�������
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ������Ӱ��
    float4 ambient = gAmbientLight * diffuseAlbedo;

	// ֻ������Դ��Ͷ��������Ӱ
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	// ��Ӱ����[0,1],����һ����,0��ʾλ����Ӱ֮��,1��ʾ��Ӱ֮��,0~1��ʾ���ִ�����Ӱ֮��
	// ���˵�����ֵ,�ֱ����Դ˵�Ϊ���ĵ�9�������Ӱͼ����ֵ�Ƚ�,����ƽ��ֵ
    shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);

    const float shininess = (1.0f - roughness) * normalMapSample.a; // TODO ʹ��bumpedNormalW
    Material mat = { diffuseAlbedo, fresnelR0, shininess };

	// ��ԴӰ��
	// ����ʹ�÷�����ͼ����
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW, bumpedNormalW, toEyeW, shadowFactor);

    float4 litColor = ambient + directLight;

	// ��պи߹ⷴ��
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;

    litColor.a = diffuseAlbedo.a;

    return litColor;
}