#include "../Public/d3dApp.h"
#include "../Public/MathHelper.h"
#include "../Public/UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX; // XMFLOAT3
using namespace DirectX::PackedVector;

// ����ṹ��
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

// �������ṹ��
struct ObjectConstants
{
	// ����-��ͼ-ͶӰ ����
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class BoxApp : public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	// = delete ���ú���
	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;
	~BoxApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	// ����Windows��Ϣʱ����
	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	// ������������
	void BuildDescriptorHeaps();
	// ��������������
	void BuildConstantBuffers();
	// ������ǩ��
	void BuildRootSignature();
	// ������ɫ�������벼��
	void BuildShadersAndInputLayout();
	// ����Box������
	void BuildBoxGeometry();
	// ������ˮ��״̬���� PSO : Pipeline State Object ǰ�漸����������Ϊ��׼��
	void BuildPSO();

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;	// ��ǩ��
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;		// ������������������

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr; // �ϴ���������������

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr; // ����������

	ComPtr<ID3DBlob> mvsByteCode = nullptr;	// ������ɫ��(vertex shader)�ֽ���(HLSL�����ļ�)
	ComPtr<ID3DBlob> mpsByteCode = nullptr;	// ������ɫ��(pixed shader)�ֽ���(HLSL�����ļ�)

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout; // ���벼��

	ComPtr<ID3D12PipelineState> mPSO = nullptr;		// ��ˮ��״̬���� PSO: Pipeline State Object

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();	// �������
	XMFLOAT4X4 mView = MathHelper::Identity4x4();	// ��ͼ����
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();	// ͶӰ����

	float mTheta = 1.5f * XM_PI; // ���ˮƽ��ת, ������������ʾ,����ת��Ϊ�ѿ�������,��BoxΪԭ��
	float mPhi = XM_PIDIV4;	// �����ֱ��ת
	float mRadius = 5.0f;	// �������۲����ľ���

	POINT mLastMousePos;	// ��һ֡���λ��
};