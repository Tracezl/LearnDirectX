#include <windows.h> // Win32 API�ṹ��,��������,��������

HWND ghMainWnd = 0; // �����ڵľ��

// ��ʼ��WindowsӦ�ó���
bool InitWindowsApp(HINSTANCE instanceHandle, int show);

// ��Ϣѭ��
int Run();

// ���ڹ���:�����������ܵ�����Ϣ
LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ��WindowsӦ�ó�����WinMain�൱�ڴ�������Ե�main()
// ���WinMain�����ɹ�����,��ô������ֹʱ,Ӧ����WM_QUIT��wParam��Ա
// ����������˳�ʱ��û������Ϣѭ��,����0
// #define WINAPI __stdcall ָ�������ĵ���Լ��,�غ�������������ջ˳���
// hInstance:��ǰӦ�ó����ʵ�����
// hPrevInstance:Win32����ò���
// pCmdLine:���д˳������õ���������ַ���
// nCmdShow:Ӧ�ó��������ʾ(���մ��ڵ�ǰ�Ĵ�С��λ��,�������...)
int WINAPI
WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// ��������ʼ��Ӧ�ó���������
	if (!InitWindowsApp(hInstance, nShowCmd))
	{
		return 0;
	}
	// ������Ϣѭ��.ֱ�����յ���ϢWM_QUIT(Ӧ�ó���ر�)
	return Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	// ��дWNDCLASS�ṹ��,������������������������һ������
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW; // ���������ʽ,���������Ŀ�Ȼ�߶ȷ����ı�ʱ���ػ洰��
	wc.lpfnWndProc = WndProc; // �����ô��ڹ��̺�����ָ��
	wc.cbClsExtra = 0; // ���������ڴ�ռ�
	wc.cbWndExtra = 0; // ���������ڴ�ռ�
	wc.hInstance = instanceHandle; // ��ǰӦ�ó���ʵ���ľ��
	wc.hIcon = LoadIcon(0, IDI_APPLICATION); // ָ��һ��ͼ��ľ��,�������Ĭ��
	wc.hCursor = LoadCursor(0, IDC_ARROW); // ָ�������ʽ���
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // ָ����ˢ�ľ��,�Դ�ָ�����ڹ������ı�����ɫ
	wc.lpszMenuName = 0; // ָ�����ڵĲ˵�
	wc.lpszClassName = L"BasicWndClass"; // �������������ṹ������� CreateWindow(para1, ...)

	// ��Windowsϵͳ��,Ϊ����WNDCLASSע��һ��ʵ��
	if (!RegisterClass(&wc))
	{
		// ��Ϣ�������Ĵ��ڵľ��, �����ı�, �����ı�, ��ʽ
		MessageBox(0, L"RegisterClass FAILED", 0, 0);
		return false;
	}

	// ����CreateWindow()��������,���ش������ڵľ��(HWND����),����ʧ�ܷ���0
	// ���ھ����һ�ִ��ڵ����÷�ʽ
	ghMainWnd = CreateWindow(
		L"BasicWndClass", // ����ǰ��ע���WNDCLASSʵ��
		L"Win32Basic", // ���ڱ���
		WS_OVERLAPPEDWINDOW, // ���ڵ���ʽ��־
		CW_USEDEFAULT, // x����
		CW_USEDEFAULT, // y����
		CW_USEDEFAULT, // ���ڿ��
		CW_USEDEFAULT, // ���ڸ߶�
		0, // �����ھ��
		0, // �˵����
		instanceHandle, // Ӧ�ó���ʵ�����
		0 // ��������
	);

	if (ghMainWnd == 0)
	{
		MessageBox(0, L"Create Window FAILED", 0, 0);
		return false;
	}

	// ��ʾ�����´���
	ShowWindow(ghMainWnd, show);
	UpdateWindow(ghMainWnd);

	return true;
}

int Run()
{
	MSG msg = { 0 };

	// �ڻ�ȡWM_QUIT��Ϣ֮ǰ,�ú�����һֱ����ѭ��.
	// GetMessage����ֻ�����յ�WM_QUIT��Ϣʱ�Ż᷵��0,����������,����-1
	// û����Ϣ����֮ʱ,GetMessage�����Ӧ�ó���ջ��������״̬
	BOOL bRet = 1; // boolReturn
	while ((bRet = GetMessage(&msg, 0, 0, 0)) != 0)
	{
		if (bRet == -1)
		{
			MessageBox(0, L"GetMessage FAILED", L"ERROR", MB_OK);
			break;
		}
		else
		{
			TranslateMessage(&msg); // ʵ�ּ��̰�����ת��,�ر��ǽ����ⰴ����Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg); // ����Ϣ���ɸ�ָ���Ĵ��ڹ���
		}
	}

	return (int)msg.wParam; // ���Ӧ�ó������WM_QUIT��Ϣ˳���˳�,��WinMain����WM_QUIT��Ϣ�Ĳ���wParam(��Ϣ����)
}

// LRESULT: ������,��ʾ�����Ƿ���óɹ�
// CALLBACK: ָ������һ���ص�����,����ʾ�ĵ���,Windowsϵͳ(����Ҫ������Ϣ��ʱ��)�Զ����ô˴��ڹ���
// hWnd: ���մ���Ϣ�Ĵ��ھ��
// msg: ��ʶ����Ϣ��Ԥ��ֵ
// wParam, lParam: �������Ϣ��صĶ�����Ϣ
LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// ����һЩ�ض�����Ϣ,������һ����Ϣ��,Ӧ����0
	switch (msg)
	{
		// ����������,������Ϣ��
	case WM_LBUTTONDOWN:
		MessageBox(0, L"Hello, World", L"Hello", MB_OK);
		return 0;

		// ����Esc��,����Ӧ�ó���������
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(ghMainWnd);
		return 0;

		// ����������Ϣ�ķ����Ƿ����˳���Ϣ,��������ֹ��Ϣѭ��
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// ������û�д������Ϣת����Ĭ�ϵĴ��ڹ���
	return DefWindowProc(hWnd, msg, wParam, lParam);
}