// ����'1'���߿�ģʽ��ʾ,���Կ�������





#include "../Public/d3dApp.h"
#include "../Public/MathHelper.h"
#include "../Public/UploadBuffer.h"
#include "../Public/GeometryGenerator.h"
#include "../Public/FrameResource.h"
#include "../Public/Waves.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;
// ������һ��cpp�ļ��ı���,ʹ��extern�ؼ���
extern const int gNumFrameResources;

// ���������״����������������ṹ,��Ӧ�ò�ͬ����ͬ

struct RenderItem
{
	RenderItem() = default;

	// �������,���������λ��,��ת,����


	XMFLOAT4X4 World = MathHelper::Identity4x4();

	// dirty flay,��ʾ�������ݸı�,��Ҫ���³���������
	// ��Ϊÿ��֡��Դ�������峣��������,������Ҫ����ÿ��֡��Դ
	// ���,���޸���������ʱ,Ӧ������NumFramesDirty = gNumFrameResource
	// ����ÿ��֡��Դ���õ�����
	int NumFramesDirty = gNumFrameResources;

	// �����Ⱦ��Ŀ��GPU������������
	UINT ObjCBIndex = -1;

	MeshGeometry* Geo = nullptr;

	// ͼԪ����
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced()����
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
};

enum class RenderLayer :int
{
	Opaque = 0,
	Count
};

class LandAndWavesApp :public D3DApp
{
public:
	LandAndWavesApp(HINSTANCE hInstance);
	LandAndWavesApp(const LandAndWavesApp& rhs) = delete; // rhs:right hand side
	LandAndWavesApp& operator=(const LandAndWavesApp& rht) = delete;
	~LandAndWavesApp();

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
	void UpdateWaves(const GameTimer& gt);

	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildLandGeometry();
	void BuildWavesGeometryBuffers();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);

	float GetHillsHeight(float x, float z) const;
	XMFLOAT3 GetHillsNormal(float x, float y) const;

private:

	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	UINT mCbvSrvDescriptorSize = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	RenderItem* mWavesRitem = nullptr;

	// ������Ⱦ��Ŀ���б�
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;

	// Render items divided by PSO.
	std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

	std::unique_ptr<Waves> mWaves;

	PassConstants mMainPassCB;

	bool mIsWireframe = false;

	XMFLOAT3 mEyePos = { 0.0f,0.0f,0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = XM_PIDIV2 - 0.1f;
	float mRadius = 50.0f;

	float mSunTheta = 1.25f*XM_PI;
	float mSunPhi = XM_PIDIV4;

	POINT mLastMousePos;
};

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// ��������ʱ�ڴ���
#if defined(DEBUG)|defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	try
	{
		LandAndWavesApp theApp(hInstance);
		if (!theApp.Initialize())
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

LandAndWavesApp::LandAndWavesApp(HINSTANCE hInstance)
	:D3DApp(hInstance)
{

}

LandAndWavesApp::~LandAndWavesApp()
{
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

bool LandAndWavesApp::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	// Ϊ��׼����ʼ������,���������б�
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	// ����, ����, ����߳�, ����ʱ��Ƶ��, �ٶ�, ����
	mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildLandGeometry();
	BuildWavesGeometryBuffers();
	BuildRenderItems();
	BuildFrameResources();
	BuildPSOs();

	// ִ�г�ʼ������
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

	// �ȴ���ʼ�����
	FlushCommandQueue();

	return true;
}

void LandAndWavesApp::OnResize()
{
	D3DApp::OnResize();

	// ���¿�߱�,����ͶӰ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void LandAndWavesApp::Update(const GameTimer& gt)
{
	OnKeyboardInput(gt);
	UpdateCamera(gt);

	// ѭ����������֡��Դ����
	mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
	mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();


	// �ȴ�GPU��ɴ���ǰ֡��Դ������,����Χ����
	if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateObjectCBs(gt);
	UpdateMainPassCB(gt);
	UpdateWaves(gt);
}

void LandAndWavesApp::Draw(const GameTimer& gt)
{
	auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;



	ThrowIfFailed(cmdListAlloc->Reset());



	if (mIsWireframe)
	{
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get()));
	}
	else
	{
		ThrowIfFailed(mCommandList->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
	}

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);


	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);


	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	// ����Ⱦ�������õĳ���������,��ÿ����Ⱦ������,ִֻ��һ��
	auto passCB = mCurrFrameResource->PassCB->Resource();
	// para1: CBVҪ�󶨵ĸ���������,���Ĵ����Ĳ�λ��
	mCommandList->SetGraphicsRootConstantBufferView(1, passCB->GetGPUVirtualAddress());

	DrawRenderItems(mCommandList.Get(), mRitemLayer[(int)RenderLayer::Opaque]);


	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));


	ThrowIfFailed(mCommandList->Close());


	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;


	mCurrFrameResource->Fence = ++mCurrentFence;




	mCommandQueue->Signal(mFence.Get(), mCurrentFence);
}

void LandAndWavesApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void LandAndWavesApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void LandAndWavesApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{

		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));


		mTheta += dx;
		mPhi += dy;


		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{

		float dx = 0.2f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.2f*static_cast<float>(y - mLastMousePos.y);


		mRadius += dx - dy;


		mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void LandAndWavesApp::OnKeyboardInput(const GameTimer& gt)
{
	if (GetAsyncKeyState('1') & 0x8000)
		mIsWireframe = true;
	else
		mIsWireframe = false;
}

void LandAndWavesApp::UpdateCamera(const GameTimer& gt)
{

	mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta);
	mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta);
	mEyePos.y = mRadius * cosf(mPhi);


	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void LandAndWavesApp::UpdateObjectCBs(const GameTimer& gt)
{
	auto currObjectCB = mCurrFrameResource->ObjectCB.get();
	for (auto& e : mAllRitems)
	{
		// ֻ�б����ı��,�Ÿ��³���������
		//ÿ��֡��Դ����Ҫ׷��
		if (e->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&e->World);

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(e->ObjCBIndex, objConstants);

			// ��һ��֡��ԴҲ��Ҫ����
			e->NumFramesDirty--;
		}
	}
}

void LandAndWavesApp::UpdateMainPassCB(const GameTimer& gt)
{
	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();

	auto currPassCB = mCurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

// ÿһ֡ģ�Ⲩ��,���¶��㻺����
void LandAndWavesApp::UpdateWaves(const GameTimer& gt)
{
	// ÿ0.25s,����һ���������
	static float t_base = 0.0f;
	if (mTimer.TotalTime() - t_base >= 0.25f)
	{
		t_base += 0.25f;

		int i = MathHelper::Rand(4, mWaves->RowCount() - 5);
		int j = MathHelper::Rand(4, mWaves->ColumnCount() - 5);

		float r = MathHelper::RandF(0.2f, 0.5f);

		mWaves->Disturb(i, j, r);
	}

	// ����ģ��Ĳ���
	mWaves->Update(gt.DeltaTime());

	// �ò��˷�����������������¶��㻺����
	auto currWavesVB = mCurrFrameResource->WavesVB.get();
	for (int i = 0; i < mWaves->VertexCount(); ++i)
	{
		Vertex v;

		v.Pos = mWaves->Position(i);
		v.Color = XMFLOAT4(DirectX::Colors::Blue);

		currWavesVB->CopyData(i, v);
	}


	// ��������Ⱦ��Ķ�̬���㻺�������õ���ǰ֡�Ķ��㻺����
	mWavesRitem->Geo->VertexBufferGPU = currWavesVB->Resource();
}

void LandAndWavesApp::BuildRootSignature()
{

	CD3DX12_ROOT_PARAMETER slotRootParameter[2];


	slotRootParameter[0].InitAsConstantBufferView(0); // ����CBV
	slotRootParameter[1].InitAsConstantBufferView(1); // ��Ⱦ����CBV


	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);


	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void LandAndWavesApp::BuildShadersAndInputLayout()
{
	mShaders["standardVS"] = d3dUtil::CompileShader(L"Shaders\\Chapter7\\color.hlsl", nullptr, "VS", "vs_5_0");
	mShaders["opaquePS"] = d3dUtil::CompileShader(L"Shaders\\Chapter7\\color.hlsl", nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};
}

void LandAndWavesApp::BuildLandGeometry()
{

}