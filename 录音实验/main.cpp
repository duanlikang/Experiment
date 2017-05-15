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

// 定义窗口的行为。hwnd为窗口句柄，uMsg为消息编码，wParam和lParam为消息的额外数据
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 应用程序起始点。hInstance为应用程序句柄，pCmdLine命令行参数，nCmdShow主窗口标志（最大化，最小化，显示等）
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	// 注册窗口类
	// 窗口类定义窗口的公共行为，每个窗口特有的数据叫做实例数据（Instance Data）
	// 每个窗口必须关联一个窗口类
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc; // 指向窗口处理函数的指针
	wc.hInstance = hInstance; // 应用程序实例的句柄
	wc.lpszClassName = CLASS_NAME; // 标志窗口类的字符串

	RegisterClass(&wc);

	// 创建窗口
	HWND hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES, // Optional window styles.
		CLASS_NAME, // 窗口类的名字
		L"录音机", // 这里是窗口的标题
		WS_OVERLAPPEDWINDOW, // 窗口外观风格

		CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, // 窗口的位置和大小，这里使用默认值

		NULL, // 父窗口（parant window）或拥有窗口（owner window）
		NULL, // 菜单（Menu）
		hInstance, // 应用程序实例的句柄
		NULL // 额外的数据，void*类型
	);

	if (hwnd == NULL) {
		return 0;
	}

	// 显示窗口
	ShowWindow(hwnd, nCmdShow);

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) // 从消息队列获取消息
	{
		TranslateMessage(&msg); // 转换键盘输入
		DispatchMessage(&msg); // 当调用该函数时，窗口间接调用WindowProc函数
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		// 创建按钮
		HWND hStartRecordBtn = CreateWindow(L"Button", L"开始录制", WS_VISIBLE | WS_CHILD,
			150, 0, 80, 50,
			hwnd, (HMENU)BTN_START_RECORD, NULL, NULL);
		HWND hStopRecordBtn = CreateWindow(L"Button", L"停止录制", WS_VISIBLE | WS_CHILD,
			250, 0, 80, 50,
			hwnd, (HMENU)BTN_STOP_RECORD, NULL, NULL);
		HWND hPlayBtn = CreateWindow(L"Button", L"开始播放", WS_VISIBLE | WS_CHILD,
			100, 100, 80, 50,
			hwnd, (HMENU)BTN_PLAY, NULL, NULL);
		HWND hPauseBtn = CreateWindow(L"Button", L"暂停播放", WS_VISIBLE | WS_CHILD,
			200, 100, 80, 50,
			hwnd, (HMENU)BTN_PAUSE, NULL, NULL);
		HWND hStopBtn = CreateWindow(L"Button", L"继续播放", WS_VISIBLE | WS_CHILD,
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
		if (MessageBox(hwnd, L"是否退出？", L"Application", MB_OKCANCEL) == IDOK)
			DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		// 将WM_QUIT消息放到消息队列，它使GetMessage()函数返回0
		PostQuitMessage(0);
		return 0;

	case WM_PAINT: {
		// 当窗口第一次显示时，会将WM_PAINT消息放入队列
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
		return 0;
	}
	}

	// 默认窗口处理程序
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
