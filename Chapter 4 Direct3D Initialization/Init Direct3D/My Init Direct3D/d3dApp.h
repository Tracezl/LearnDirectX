#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "../../../Common/d3dUtil.h"
#include "../../../Common/GameTimer.h"

// ����DirectX��
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class D3DApp
{
protected:

	D3DApp(HINSTANCE hInstance); // HINSTANCE�Ǿ����������,�൱��װ�뵽���ڴ����Դid,ʵ�����޷��ų�����
	D3DApp(const D3DApp& rhs) = delete; // ��ֹʹ�ñ�����Ĭ�����ɵĺ���
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:

	static D3DApp* GetApp();

	HINSTANCE AppInst() const; // ����Ӧ�ó���ʵ�����
	HWND MainWnd() const; // ���������ھ�� HWND:���ھ��
	float AspectRatio() const; // ��̨�������Ŀ�߱�

	bool Get4xMsaaState() const; // �Ƿ���4X MSAA(���ز��������)
	void Set4xMsaaState(bool value);

	int Run(); // ��װӦ�ó������Ϣѭ��,ʹ��Win32��PeekMessage����,��û�д�����Ϣʱ,�ͻᴦ����Ϸ�߼�



	virtual bool Initialize(); // ��ʼ��,�������Դ,��ʼ������,����3D������
	// Ӧ�ó��������ڵĴ��ڹ��̺��� LRESULT: ���ڳ����ص��������ص�32λֵ,WPARAM,LPARAM: ��Ϣ��Ӧ����
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam); 

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps(); // ����RTV��DSV��������
	virtual void OnResize(); // ��D3DApp::MsgProc�������յ�WM_SIZE��Ϣʱ����
	virtual void Update(const GameTimer& gt) = 0; // ÿ֡����
	virtual void Draw(const GameTimer& gt) = 0; // ÿ֡����,������Ⱦ����,����ǰ֡���Ƶ���̨������



	// �����������
	virtual void OnMouseDown(WPARAM btnState, int x, int y) {} // WPARAM:����¼�����ʱ,�ĸ�������
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}

protected:

	bool InitMainWindow(); // ��ʼ��Ӧ�ó���������
	bool InitDirect3D();
	void CreateCommandObjects(); // �����������,����������������б�
	void CreateSwapChain(); //����������

	void FlushCommandQueue(); // ǿ��CPU�ȴ�GPU,ֱ��GPU����������е���������

	ID3D12Resource* CurrentBackBuffer() const; // ���ص�ǰ��̨��������ID3D12Resource
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const; // ���ص�ǰ��̨��������RTV(��ȾĿ����ͼ)
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const; // ���������/ģ�建������DSV(���/ģ����ͼ)

	void CalculateFrameStats(); // ����ÿ���ƽ��֡��,ÿ֡ƽ���ĺ�����

	void LogAdapters(); // ö��ϵͳ�����е�������(�Կ�)
	void LogAdapterOutputs(IDXGIAdapter* adapter); // ö��ָ����������ȫ����ʾ���(��ʾ��)
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format); // ö��ĳ����ʾ������ض���ʽ֧�ֵ�������ʾģʽ

protected:

	static D3DApp* mApp;

	HINSTANCE	mhAppInst = nullptr;	// Ӧ��ʵ�����
	HWND		mhMainWnd = nullptr;	// �����ھ��
	bool		mAppPaused = false;		// �����Ƿ���ͣ
	bool		mMinimized = false;		// �����Ƿ���С��
	bool		mMaximized = false;		// �����Ƿ����
	bool		mResizing = false;		// �Ƿ�������ק�ߴ���
	bool		mFullScreenState = false; // �Ƿ�����ȫ��
	bool		m4xMsaaState = false;	// �Ƿ�����4X MSAA(4�����ز��������)
	UINT		m4xMsaaQuality = 0;		// 4X MSAA�ȼ�

	GameTimer mTimer;

	// COM:Component Object Model �������ģ��,��һ����DirectX���ܱ����������,��ʹ֮�����ݵļ���
	// Ϊ����COM�������������,Windows����ʱ��(WRL:Windows Runtime Library)�ṩ��Microsoft::WRL::ComPtr��
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory; // ���ڴ���������,����WARP(Windows�߼���դ��ƽ̨)
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain; // ������
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice; // ��ʾ������(�Կ�)

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence; // Χ��
	UINT64 mCurrentFence = 0; // ��ǰΧ��ֵ

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue; // GPU�������
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc; // ���������
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList; // CPU�����б�

	static const int SwapChainBufferCount = 2; // �м���������(ǰ+��)
	int mCurrBackBuffer = 0; // ��ǰ��̨������
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount]; // ��ȾĿ����ͼ������
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer; // ���/ģ�建����

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap; // RTV��������
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap; // DSV��������

	D3D12_VIEWPORT mScreenViewport; // �ӿ�
	D3D12_RECT mScissorRect; // �ü�����

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// �̳���Ӧ���ڹ�������������Щֵ
	std::wstring mMainWndCaption = L"d3d App"; // wstring:���ַ�, caption:˵������
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800; // ���ڿ��
	int mClientHeight = 600; // ���ڸ߶�
};