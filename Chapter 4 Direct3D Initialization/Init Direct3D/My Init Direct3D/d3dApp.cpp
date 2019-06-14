#include "d3dApp.h"
#include <windowsx.h> // GET_X_LPARAM

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp* D3DApp::mApp = nullptr; // Ӧ��ʵ��
D3DApp* D3DApp::GetApp()
{
	return mApp;
}

D3DApp::D3DApp(HINSTANCE hInstance)
	:mhAppInst(hInstance)
{
	// ֻ��һ��D3DApp���Ա�����
	assert(mApp == nullptr);
	mApp = this;
}

D3DApp::~D3DApp()
{
	// ������GPU���õ���Դ��ǰ,����ȴ�GPU����������е���������.����������Ӧ�ó������˳�ʱ����
	if (md3dDevice != nullptr)
		FlushCommandQueue();
}

HINSTANCE D3DApp::AppInst() const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd() const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio() const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

bool D3DApp::Get4xMsaaState() const
{
	return m4xMsaaState;
}

void D3DApp::Set4xMsaaState(bool value)
{
	if (m4xMsaaState != value)
	{
		m4xMsaaState = value;

		// ��϶��ز���,���´����������ͻ�����
		CreateSwapChain();
		OnResize();
	}
}

int D3DApp::Run()
{
	MSG msg = { 0 };

	mTimer.Reset(); // ������Ϣѭ��֮ǰ,������Ϸʱ��

	while (msg.message != WM_QUIT)
	{
		// �����Window��Ϣ,����
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// ����,������Ϸ�߼�
		else
		{
			mTimer.Tick();

			if (!mAppPaused) // ��Ϸ��Ҫ�߼�
			{
				CalculateFrameStats();
				Update(mTimer);
				Draw(mTimer);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool D3DApp::Initialize()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	// ��ʼ��resize
	OnResize();

	return true;
}

void D3DApp::CreateRtvAndDsvDescriptorHeaps()
{
	// ����RTV��������,SwapChainBufferCount��
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));

	// ����DSV��������,1��
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void D3DApp::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	// �ı���Դǰ,��Flush
	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// �ͷŽ�Ҫ���´�����֮ǰ����Դ
	for (int i = 0; i < SwapChainBufferCount; i++)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	// resize ������
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth, mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	// ��Դ��������Ⱦ��ˮ���еĽ׶�ֱ�Ӱ�,���Ա�����Ϊ��Դ������ͼ(������)
	// �������Ѿ��
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		// Ϊ�˽���̨�������󶨵���ˮ�ߵ�����ϲ��׶�
		// ��ҪΪ��̨����������RTV
		// ��һ������:��ȡ�������еĻ�������Դ
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]))); // ��̨������������:i
		// Ϊ��ȡ�ĺ�̨������������ȾĿ����ͼ
		// para2: D3D12_RENDER_TARGET_VIEW_DESC,��������Դ��Ԫ�ص���������,�������Դ�ڴ���ʱ��ָ���˾����ʽ,�Ϳ�����Ϊnullptr
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		// ƫ�Ƶ����������е���һ��������
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	// ���/ģ�建���������ṹ��
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // ��Դά��
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth; // ������
	depthStencilDesc.Height = mClientHeight; // ����߶�
	depthStencilDesc.DepthOrArraySize = 1; // �������(����2D����,����������Ĵ�С)
	depthStencilDesc.MipLevels = 1; // mipmap�㼶����,�������/ģ�建����,1
	depthStencilDesc.Format = mDepthStencilFormat; // DXGI_FORMAT_D24_UNORM_S8_UINT. 24λ���,ӳ�䵽[0,1], 8λģ��,ӳ�䵽[0,255]
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // ָ��������
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // ����Դ�йص������־

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	// GPU��Դ�����ڶ�(heap)��,�����Ǿ����ض����Ե�GPU�Դ��
	// ID3D12Device::CreateCommittedResource����,���������ṩ �����Դ���һ����Դ��һ����,���Ѹ���Դ�ύ���������

	// �������ģ�建����
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), // (��Դ���ύ����)�������еĵ�����,���ģ�建�����ϴ���Ĭ�϶�.Ĭ�϶ѵ���Դֻ��GPU���Է���
		D3D12_HEAP_FLAG_NONE, // ����йصĶ���ѡ���־
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON, // ��Դ����ʱ�ĳ�ʼ״̬
		&optClear, // �������������Դ���Ż�ֵ
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	// ����DSV
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, DepthStencilView());

	// ����Դ�ӳ�ʼ״̬ת��Ϊ��Ȼ���
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// ִ�� resize ����
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists); // �������б����������ӵ����������

	// �ȴ� resize ���
	FlushCommandQueue();

	// �����ӿ�λ��
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0,0,mClientWidth,mClientHeight };
}

LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// ���ڱ�����򷴼���ʱ,WM_ACTIVATE������
		// ������ʱ��ͣ��Ϸ,����ʱȡ����ͣ
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// �û�resize����
	case WM_SIZE:
		// �����µĳߴ�
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (md3dDevice)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				// ����С���ָ�
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}
				// ����󻯻ָ�
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// ����WM_EXITSIZEMOVE����,����᲻�ϴ���
				}
				else // ����API,��SetWindowPos,mSwapChain->SetFullscreenState
				{
					OnResize();
				}
			}
		}
		return 0;

		// �û���ʼ��ק
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		// �û�������ק
		// ���û��ڴ��ڳߴ������,�绺����,��ͼ��
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// ���ڱ��ݻ�
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// ����˵�,�����û����µļ�����Ӧ���Ǽ�����ټ�
	case WM_MENUCHAR:
		// alt-enter ������������
		return MAKELRESULT(0, MNC_CLOSE);

		// ץס������Ϣ�Է����ڱ��̫С
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE) // ����Esc
		{
			PostQuitMessage(0);
		}
		else if ((int)wParam == VK_F2) // ����F2(������������)
			Set4xMsaaState(!m4xMsaaState);

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool D3DApp::InitMainWindow()
{
	// ��дWNDCLASS�ṹ��
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc; // �������ڹ��̺�����ָ��
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd"; // �������������ṹ�������

	// ע��ṹ��
	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return false;
	}

	// ���㴰�ڳߴ�
	RECT R = { 0,0,mClientWidth,mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	// ��������
	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

bool D3DApp::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	// ����D3D12 Debug
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	// Ϊ����WARP������(Ҳ�����ڴ���������),��Ҫ�ȴ���һ��IDXGIFactory4����
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// 1,���Դ���Ӳ���豸
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,	// ָ�������豸ʱ���õ���ʾ������(�Կ�). nullptr:ʹ������ʾ������
		D3D_FEATURE_LEVEL_11_0, // Ӧ�ó�����ҪӲ����֧�ֵ���͹��ܼ���.�����������֧�ִ˹��ܼ���,���豸����ʧ��.������Direct3D11
		IID_PPV_ARGS(&md3dDevice));

	// �ع���WARP�豸
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWrapAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWrapAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWrapAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&md3dDevice)));
	}

	// 2-1,����Χ��
	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, // ��ʼֵ:0
		IID_PPV_ARGS(&mFence)));
	// 2-2,��ȡ�������Ĵ�С
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 3,��� 4X MSAA ����֧��
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	// 4,�����������,�����б�,���������
	CreateCommandObjects();
	// 5,����������
	CreateSwapChain();
	// 6,7,8 ������������,������,RTV,DSV
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void D3DApp::CreateCommandObjects()
{
	// �����������
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	// �������������
	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, // �ɹ�GPUֱ��ִ�е�����
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));

	// ���������б�
	ThrowIfFailed(md3dDevice->CreateCommandList(
		0, // node mask, ָ�������������б������������GPU, ���ڵ�GPU��ϵͳ,����Ϊ0
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(),	// ������command allocator
		nullptr,					// ��ʼPipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// �״����������б�ʱ��Ҫ����,������ǰ��Ҫ�ر�
	mCommandList->Close();
}

void D3DApp::CreateSwapChain()
{
	// �ͷ�֮ǰ�Ľ�����
	mSwapChain.Reset();

	// ��дDXGI_SWAP_CHAIN_DESC�ṹ��
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth; // �������ֱ��ʵĿ��
	sd.BufferDesc.Height = mClientHeight; // �������ֱ��ʵĸ߶�
	sd.BufferDesc.RefreshRate.Numerator = 60;	// ����
	sd.BufferDesc.RefreshRate.Denominator = 1;	// ��ĸ
	sd.BufferDesc.Format = mBackBufferFormat; // ����������ʾ��ʽ,������DXGI_FORMAT_R8G8B8A8_UNORM
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // ����ɨ�� vs ����ɨ��
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // ͼ������������Ļ��������
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1; // ���ز���ÿ�����صĲ�������
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0; // ���ز�������������
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // ��������Ҫ��������Ⱦ����̨������(��������Ϊ��ȾĿ��),�������ָ��
	sd.BufferCount = SwapChainBufferCount; // �����������õĻ���������
	sd.OutputWindow = mhMainWnd; // ��Ⱦ���ڵľ��
	sd.Windowed = true; // ����ģʽ vs ȫ��ģʽ
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // �л�ȫ��ʱ,ѡ�����ʺ��ڵ�ǰӦ�ó��򴰿ڳߴ����ʾģʽ

	// ����������
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()));
}

void D3DApp::FlushCommandQueue()
{
	mCurrentFence++;

	// ��������м���ָ��,�������µ�Χ��ֵ
	// ֱ��GPU�������������Signal()������,������
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	// �ȴ�ֱ��GPU������е����Χ��ֵ������
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// ��GPU���ﵱǰΧ��ֵʱ(ִ�е�Signal()ָ��,�޸���Χ��ֵ),����Ԥ���¼�
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// �ȴ�ֱ��GPU���ﵱǰΧ��ֵ,�����¼�
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

ID3D12Resource* D3DApp::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const
{
	// �����������Ѻ�,�������������������
	// ͨ���������������
	// ������SwapChainBufferCount��RTV
	// ��ƫ�����ҵ���ǰ��̨��������RTV������,����֪��RTV�Ĵ�С
	// α����: Ŀ����������� = GetCPUDescriptorHandleForHeapStart() + mCurrBackBuffer * mRtvDescriptorSize
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(), // ������������е�һ���������ľ��
		mCurrBackBuffer, // ƫ������̨���������������������
		mRtvDescriptorSize); // ��������ռ�ֽڵĴ�С
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
	// ֻ������1��DSV
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3DApp::CalculateFrameStats()
{
	// ͳ���������ڴ���˵����
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// TotalTimeÿ֡����,����ʱ������1sΪ����ͳ��
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;	// frameCnt/1
		float mspf = 1000.0f / fps;		// ÿ֡����ms

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = mMainWndCaption +
			L"		fps: " + fpsStr +
			L"	   mspf: " + mspfStr;

		// ���ô������Ͻ����� 
		SetWindowText(mhMainWnd, windowText.c_str());

		// ���ô���һ������
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void D3DApp::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void D3DApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void D3DApp::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// ��ȡcount
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}