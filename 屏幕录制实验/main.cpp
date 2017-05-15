#ifndef UNICODE
#define UNICODE
#endif // !UNICODE
#include <Windows.h>
#include <mmsystem.h>
#include <Vfw.h>//注意引用顺序
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "vfw32.lib")
#define BTN_START_CAPTURE 1 
#define BTN_STOP_CAPTURE 2

bool is_paused = false;
HWND hWndC;//捕获窗口行为
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
		L"视频捕捉", // 这里是窗口的标题
		WS_OVERLAPPEDWINDOW, // 窗口外观风格

		CW_USEDEFAULT, CW_USEDEFAULT, 500, 600, // 窗口的位置和大小，这里使用默认值

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
		HWND hStartRecordBtn = CreateWindow(L"Button", L"开始捕获", WS_VISIBLE | WS_CHILD,
			100, 420, 80, 50,
			hwnd, (HMENU)BTN_START_CAPTURE, NULL, NULL);
		HWND hStopRecordBtn = CreateWindow(L"Button", L"停止捕获", WS_VISIBLE | WS_CHILD,
			250, 420, 80, 50,
			hwnd, (HMENU)BTN_STOP_CAPTURE, NULL, NULL);
		hWndC = capCreateCaptureWindow((LPCWSTR)"Capture Window",

			WS_CHILD | WS_VISIBLE, 0, 0, 500,400, (HWND)hwnd, (int)0);
		//设置异步
		CAPTUREPARMS CaptureParams;
		capCaptureGetSetup(hWndC, &CaptureParams, sizeof(CAPTUREPARMS));
		CaptureParams.fYield = TRUE;
		capCaptureSetSetup(hWndC, &CaptureParams, sizeof(CAPTUREPARMS));
		capDriverConnect(hWndC, 0);//连接驱动


		WCHAR szCaptureFile[] = L"Capture.AVI";//转成unicode，防止乱码
		capFileSetCaptureFile(hWndC, szCaptureFile);//命名捕获文件
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
			//开始捕获
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
		if (MessageBox(hwnd, L"是否退出?", L"Application", MB_OKCANCEL) == IDOK)
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
