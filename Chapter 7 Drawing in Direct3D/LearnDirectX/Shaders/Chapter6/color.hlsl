// ��6�� ��ɫ������
// ����: error X3000: Illegal character in shader file
// ���: �ر�UTF-8���,ò��shader�����ʽֻ��ΪANSI?


// ��������������Դ�󶨵�����������(cbuffer)�Ĵ�����0
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL	: POSITION; // ��������:POSITION,������ṹ���е�Ԫ��ӳ�䵽������ɫ������Ӧ���뺯��
	float4 Color: COLOR;	// ��������:COLOR
};

struct VertexOut
{
	float4 PosH	: SV_POSITION; // SV: System Value,����Ϊ���λ����Ϣ�Ĳ�������SV_POSITION����
	float4 Color: COLOR;
};
// ������ɫ�����
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// �Ѷ���任����βü��ռ�
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// ֱ�ӽ��������ɫ��Ϣ����������ɫ��
	vout.Color = vin.Color;

	return vout;
}
// ������ɫ�����
float4 PS(VertexOut pin) : SV_Target // SV_Target�����ʾ����ֵ����������ȾĿ���ʽ(render target format)ƥ��
{
	return pin.Color;
}
