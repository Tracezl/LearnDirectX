#include "../../../Common/d3dApp.h"
#include "../../../Common/MathHelper.h"
#include "../../../Common/UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class BoxApp : public D3DApp
{
public:
	BoxApp(HINSTANCE hInstance);
	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;
	~BoxApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void BuildDescriptorHeaps(); // ����������������������,Ϊ mCbvHeap ��ֵ, RTV,DSV �����������ڻ����ʼ��
	void BuildConstantBuffers(); // Ϊ mObjectCB ����ռ�,���������� mCbvHeap ��
	void BuildRootSignature(); // ��ǩ��ָ���˳����������󶨵��ĸ���ɫ���Ĵ���
	void BuildShadersAndInputLayout(); // ָ�� VS, PS, mInputLayout(����ṹ��ӳ�䵽VS���������)
	void BuildBoxGeometry(); // ������/�����ϴ���Ĭ�϶�, �浽 mBoxGeo ��
	void BuildPSO(); // Ϊ mPSO ��ֵ

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr; // ��ǩ��,ָ������ɫ�������������Դ(CBV��Ӧ�ĸ���ɫ���Ĵ���)
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr; // ����������������

	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr; // ����������, Update ��,ÿ֡���±任����

	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr; // ����õĶ�����ɫ���ֽ���
	ComPtr<ID3DBlob> mpsByteCode = nullptr; // ����õ�������ɫ���ֽ���

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout; // ���벼������,������ṹ��ӳ�䵽VS�����������

	ComPtr<ID3D12PipelineState> mPSO = nullptr; // ��ˮ��״̬����,������ mRootSignature, mInputLayout, mvsByteCode, mpsByteCode

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;
};
