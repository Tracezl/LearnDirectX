// ��ʽ�ο� Notes/��8�� ����.md

#define MaxLights 16

struct Light
{
	float3 Strength;
	float FalloffStart;	// ���Դ,�۹��
	float3 Direction;	// �����Դ,�۹��
	float FalloffEnd;	// ���Դ,�۹��
	float3 Position;	// ���Դ
	float SpotPower;
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;	// ����� = 1 - �ֲڶ�
};

// ����˥������
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	// saturate:���ʹ�������1�ͱ��1��С��0�ͱ��0
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// ���㷴���,R0(���ʵ�һ������) = ((n-1)/(n+1))^2,n��������
// ��������������ѧ����������������߱�����İٷֱ�
// ����ʯ��˽��Ʒ��������������
// RF(��i)=RF(0��) + (1 - RF(0��))(1 - cos��)^5
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec)); // �������������ļн�

	float f0 = 1.0f - cosIncidentAngle;
	float reflectPercent = R0 + (1.0f - R0)*(f0*f0*f0*f0*f0);

	return reflectPercent;
}

// ������+���淴��
// Cs=max(L��n,0)��BL (x) RF(��h) * (m+8)/8 * (n��h)^m
float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
	// m�ɹ�����Ƶ�����,����ȸ��ݴֲڶ����
	const float m = mat.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + lightVec); // ��h=<lightVec, halfVec>

	float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

	float3 specAlbedo = fresnelFactor * roughnessFactor; // Albedo:������

	// ��ֹ���>1,�ʵ���С
	specAlbedo = specAlbedo / (specAlbedo + 1.0f);

	return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

// �����Դ
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
	// ��������⴫���ķ����෴
	float3 lightVec = -L.Direction;

	// �ʲ����Ҷ���
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// ���Դ
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	// ����ָ���Դ������
	float3 lightVec = L.Position - pos;

	// ���浽��Դ�ľ���
	float d = length(lightVec);

	// ��Χ���
	if (d > L.FalloffEnd)
		return 0.0f;

	// �淶��������
	lightVec /= d;

	// ���������Ҷ��� max(L��n,0)
	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	// ���ݾ������˥��
	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// �۹��
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
	// ����ָ���Դ������
	float3 lightVec = L.Position - pos;

	// The distance from surface to light. 
	float d = length(lightVec);

	// Range test.
	if (d > L.FalloffEnd)
		return 0.0f;

	// �淶��
	lightVec /= d;

	float ndotl = max(dot(lightVec, normal), 0.0f);
	float3 lightStrength = L.Strength * ndotl;

	// ˥��
	float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
	lightStrength *= att;

	// Scale by spotlight
	// max(cos��,0)^s
	float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

// ���ֹ��յ���
float4 ComputeLighting(Light gLights[MaxLights], Material mat,
	float3 pos, float3 normal, float3 toEye,
	float3 shadowFactor)
{
	float3 result = 0.0f;

	int i = 0;

#if (NUM_DIR_LIGHTS > 0)
	for (i = 0; i < NUM_DIR_LIGHTS; ++i)
	{
		result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
	}
#endif

#if (NUM_POINT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
	{
		result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
	}
#endif

#if (NUM_SPOT_LIGHTS > 0)
	for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
	{
		result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
	}
#endif 

	return float4(result, 0.0f);
}