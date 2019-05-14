



// ��'1'���߿�ģʽ��ʾ

#include "d3dApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "GeometryGenerator.h"
#include "FrameResource.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3; // ֡��Դ����


// �������ͼ����������������ṹ��,��Ӧ�ò�ͬ���������
struct RenderItem
{
	RenderItem() = default;

	// ��������ֲ��ռ����������ռ���������
	// ������������������ռ��е�λ��,��ת������

	XMFLOAT4X4 World = MathHelper::Identity4x4();

	// ���Ѹ��±�־dirtyflag����ʾ�������������ѷ����ı�,��Ҫ���³���������
	// ��Ϊÿ��֡��Դ����һ�����峣��������,���Զ�ÿ��֡��Դ����и���
	// ���޸����ݵ�ʱ��,����NumFramesDirty = gNumFrameResources,�Ӷ�ʹÿ��֡��Դ���õ�����

	int NumFramesDirty = gNumFrameResources;

	// ָ��GPU����������������,��Ӧ�ڵ�ǰ��Ⱦ��
	UINT ObjCBIndex = 1;

	MeshGeometry* Geo = nullptr;

	// ͼԪ����
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced�����Ĳ���
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

class ShapesApp : public D3DApp
{
public:
	ShapesApp(HINSTANCE hInstance);
	ShapesApp(const ShapesApp& rhs) = delete;
	ShapesApp& operator=(const ShapesApp& rhs) = delete;
	~ShapesApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void OnKeyboardInput(const GameTimer& gt);
	void UpdateCamera(const GameTimer& gt);
	void UpdateObjectCBs(const GameTimer& gt);
	void UpdateMainPassCB(const GameTimer& gt);

	void BuildDescriptorHeaps();
	void BuildConstantBufferViews();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

private:

	// ��3��֡��Դ���ɵ�����
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	// ��ǰ��֡��Դ
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr; // ��ǩ��
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr; // ������������

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr; // ��ɫ����Դshader resource��������

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries; // map: string -> MeshGeometry
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders; // map: string -> ID3DBlob
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs; // map: string -> ID3D12PipelineState

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// ��Ⱦ��Ŀ�б�
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO
	std::vector<RenderItem*> mOpaqueRitems;

	PassConstants mMainPassCB;

	UINT mPassCbvOffset = 0;

	bool mIsWireframe = false;

	XMFLOAT3 mEyePos = { 0.0f,0.0f,0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = 0.2f * XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;
};
