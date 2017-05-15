#ifndef UNICODE
#define UNICODE
#endif // !UNICODE
#include <Windows.h>
#include <mmsystem.h>
#include <Vfw.h>//ע������˳��
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "vfw32.lib")
#define BTN_START_CAPTURE 1 
#define BTN_STOP_CAPTURE 2

bool is_paused = false;
HWND hWndC;//���񴰿���Ϊ
// ���崰�ڵ���Ϊ��hwndΪ���ھ����uMsgΪ��Ϣ���룬wParam��lParamΪ��Ϣ�Ķ�������
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Ӧ�ó�����ʼ�㡣hInstanceΪӦ�ó�������pCmdLine�����в�����nCmdShow�����ڱ�־����󻯣���С������ʾ�ȣ�
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	// ע�ᴰ����
	// �����ඨ�崰�ڵĹ�����Ϊ��ÿ���������е����ݽ���ʵ�����ݣ�Instance Data��
	// ÿ�����ڱ������һ��������
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc; // ָ�򴰿ڴ�������ָ��
	wc.hInstance = hInstance; // Ӧ�ó���ʵ���ľ��
	wc.lpszClassName = CLASS_NAME; // ��־��������ַ���

	RegisterClass(&wc);

	// ��������
	HWND hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES, // Optional window styles.
		CLASS_NAME, // �����������
		L"��Ƶ��׽", // �����Ǵ��ڵı���
		WS_OVERLAPPEDWINDOW, // ������۷��

		CW_USEDEFAULT, CW_USEDEFAULT, 500, 600, // ���ڵ�λ�úʹ�С������ʹ��Ĭ��ֵ

		NULL, // �����ڣ�parant window����ӵ�д��ڣ�owner window��
		NULL, // �˵���Menu��
		hInstance, // Ӧ�ó���ʵ���ľ��
		NULL // ��������ݣ�void*����
	);

	if (hwnd == NULL) {
		return 0;
	}

	// ��ʾ����
	ShowWindow(hwnd, nCmdShow);

	// ��Ϣѭ��
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) // ����Ϣ���л�ȡ��Ϣ
	{
		TranslateMessage(&msg); // ת����������
		DispatchMessage(&msg); // �����øú���ʱ�����ڼ�ӵ���WindowProc����
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		// ������ť
		HWND hStartRecordBtn = CreateWindow(L"Button", L"��ʼ����", WS_VISIBLE | WS_CHILD,
			100, 420, 80, 50,
			hwnd, (HMENU)BTN_START_CAPTURE, NULL, NULL);
		HWND hStopRecordBtn = CreateWindow(L"Button", L"ֹͣ����", WS_VISIBLE | WS_CHILD,
			250, 420, 80, 50,
			hwnd, (HMENU)BTN_STOP_CAPTURE, NULL, NULL);
		hWndC = capCreateCaptureWindow((LPCWSTR)"Capture Window",

			WS_CHILD | WS_VISIBLE, 0, 0, 500,400, (HWND)hwnd, (int)0);
		//�����첽
		CAPTUREPARMS CaptureParams;
		capCaptureGetSetup(hWndC, &CaptureParams, sizeof(CAPTUREPARMS));
		CaptureParams.fYield = TRUE;
		capCaptureSetSetup(hWndC, &CaptureParams, sizeof(CAPTUREPARMS));
		capDriverConnect(hWndC, 0);//��������


		WCHAR szCaptureFile[] = L"Capture.AVI";//ת��unicode����ֹ����
		capFileSetCaptureFile(hWndC, szCaptureFile);//���������ļ�
		capFileAlloc(hWndC, (1024L * 1024L * 5));

		capPreviewRate(hWndC, 66); // rate, in milliseconds
		capPreviewScale(hWndC, TRUE);
		capPreview(hWndC, TRUE); // starts preview
		return 0;
	}

	case WM_COMMAND: {
		TCHAR buffer[128];
		switch (LOWORD(wParam)) {
		case BTN_START_CAPTURE:
			//��ʼ����
			capCaptureSequence(hWndC);
			break;
		case BTN_STOP_CAPTURE:
			// MessageBox(hwnd, L"Stop record!", L"My Application", MB_OK);
			capCaptureAbort(hWndC);
			break;
		default:
			break;
		}
		return 0;
	}

	case WM_CLOSE:
		if (MessageBox(hwnd, L"�Ƿ��˳�?", L"Application", MB_OKCANCEL) == IDOK)
			DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		// ��WM_QUIT��Ϣ�ŵ���Ϣ���У���ʹGetMessage()��������0
		PostQuitMessage(0);
		return 0;

	case WM_PAINT: {
		// �����ڵ�һ����ʾʱ���ὫWM_PAINT��Ϣ�������
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	}

	// Ĭ�ϴ��ڴ������
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
