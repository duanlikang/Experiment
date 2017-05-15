#ifndef UNICODE
#define UNICODE
#endif // !UNICODE

#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define BTN_START_RECORD 1
#define BTN_STOP_RECORD 2
#define BTN_PLAY 3
#define BTN_PAUSE 4
#define BTN_RESUME 5

bool is_paused = false;

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
		L"¼����", // �����Ǵ��ڵı���
		WS_OVERLAPPEDWINDOW, // ������۷��

		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, // ���ڵ�λ�úʹ�С������ʹ��Ĭ��ֵ

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
		HWND hStartRecordBtn = CreateWindow(L"Button", L"��ʼ¼��", WS_VISIBLE | WS_CHILD,
			150, 0, 80, 50,
			hwnd, (HMENU)BTN_START_RECORD, NULL, NULL);
		HWND hStopRecordBtn = CreateWindow(L"Button", L"ֹͣ¼��", WS_VISIBLE | WS_CHILD,
			250, 0, 80, 50,
			hwnd, (HMENU)BTN_STOP_RECORD, NULL, NULL);
		HWND hPlayBtn = CreateWindow(L"Button", L"��ʼ����", WS_VISIBLE | WS_CHILD,
			100, 100, 80, 50,
			hwnd, (HMENU)BTN_PLAY, NULL, NULL);
		HWND hPauseBtn = CreateWindow(L"Button", L"��ͣ����", WS_VISIBLE | WS_CHILD,
			200, 100, 80, 50,
			hwnd, (HMENU)BTN_PAUSE, NULL, NULL);
		HWND hStopBtn = CreateWindow(L"Button", L"��������", WS_VISIBLE | WS_CHILD,
			300, 100, 80, 50,
			hwnd, (HMENU)BTN_RESUME, NULL, NULL);
		return 0;
	}

	case WM_COMMAND: {
		TCHAR buffer[128];
		switch (LOWORD(wParam)) {
		case BTN_START_RECORD:
			mciSendString(L"open new type waveaudio alias capture", buffer, 128, NULL);
			mciSendString(L"record capture overwrite", buffer, 128, NULL);
			break;
		case BTN_STOP_RECORD:
			mciSendString(L"stop capture", buffer, 128, NULL);
			mciSendString(L"save capture music.wav", buffer, 128, NULL);
			break;
		case BTN_PLAY:
			mciSendString(L"play capture from 0", buffer, 128, NULL);
			break;
		case BTN_PAUSE:
			mciSendString(L"pause capture", buffer, 128, NULL);
			break;
		case BTN_RESUME:
			mciSendString(L"resume capture", buffer, 128, NULL);
			break;
		default:
			break;
		}
		return 0;
	}

	case WM_CLOSE:
		if (MessageBox(hwnd, L"�Ƿ��˳���", L"Application", MB_OKCANCEL) == IDOK)
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
