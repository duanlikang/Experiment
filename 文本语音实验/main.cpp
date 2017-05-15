#include <Windows.h>
#include <sapi.h>
#include <sphelper.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "sapi.lib")
#pragma warning(disable: 4996)
HINSTANCE g_Instance = NULL;

#define BTN_PLAY 1
#define BTN_PAUSE 2
#define BTN_RESUME 3
#define LIST_BOX 4

HWND hwnd;
HWND hEdit;
HWND hPlayBtn;
HWND hPauseBtn;
HWND hResumeBtn;
HWND hList;

ISpVoice* pVoice = NULL;

bool paused = false;

// 定义窗口的行为。hwnd为窗口句柄，uMsg为消息编码，wParam和lParam为消息的额外数据
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 应用程序起始点。hInstance为应用程序句柄，pCmdLine命令行参数，nCmdShow主窗口标志（最大化，最小化，显示等）
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	g_Instance = hInstance;

	if (FAILED(CoInitialize(NULL)))
		return -1;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);//注册pvoice
	if (FAILED(hr)) {
		return -1;
	}
	else if (SUCCEEDED(hr)) {
		//将输出绑定到文件里面
		//CComPtr<ISpStream> cpWavStream;
		//CComPtr<ISpStreamFormat> cpOldStream;
		//CSpStreamFormat OriginalFmt;
		//pVoice->GetOutputStream(&cpOldStream);
		//OriginalFmt.AssignFormat(cpOldStream);
		//hr = SPBindToFile(L"output.wav", SPFM_CREATE_ALWAYS,
		//	&cpWavStream, &OriginalFmt.FormatId(),
		//	OriginalFmt.WaveFormatExPtr());
		//if (SUCCEEDED(hr))
		//{
		//	pVoice->SetOutput(cpWavStream, TRUE);
		//}
	}

	// 注册窗口类
	// 窗口类定义窗口的公共行为，每个窗口特有的数据叫做实例数据（Instance Data）
	// 每个窗口必须关联一个窗口类
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc; // 指向窗口处理函数的指针
	wc.hInstance = hInstance; // 应用程序实例的句柄
	wc.lpszClassName = CLASS_NAME; // 标志窗口类的字符串
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	RegisterClass(&wc);

	// 创建窗口
	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES, // Optional window styles.
		CLASS_NAME, // 窗口类的名字
		L"文本朗读程序", // 这里是窗口的标题
		WS_OVERLAPPEDWINDOW, // 窗口外观风格

		CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, // 窗口的位置和大小，这里使用默认值

		NULL, // 父窗口（parant window）或拥有窗口（owner window）
		NULL, // 菜单（Menu）
		hInstance, // 应用程序实例的句柄
		NULL // 额外的数据，void*类型
	);

	if (hwnd == NULL) {
		return 0;
	}

	// 显示主窗口
	ShowWindow(hwnd, nCmdShow);

	// 消息循环
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) // 从消息队列获取消息
	{
		TranslateMessage(&msg); // 转换键盘输入
		DispatchMessage(&msg); // 当调用该函数时，窗口间接调用WindowProc函数
	}

	CoUninitialize();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		hEdit = CreateWindowEx(NULL, L"Edit", L"请输入要读的内容", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_MULTILINE,
			50, 70, 500, 400, hwnd, NULL, g_Instance, NULL);
		hPlayBtn = CreateWindowEx(NULL, L"Button", L"播放", WS_CHILD | WS_VISIBLE,
			160, 10, 80, 50, hwnd, (HMENU)BTN_PLAY, g_Instance, NULL);
		hPauseBtn = CreateWindowEx(NULL, L"Button", L"暂停", WS_CHILD | WS_VISIBLE,
			260, 10, 80, 50, hwnd, (HMENU)BTN_PAUSE, g_Instance, NULL);
		hResumeBtn = CreateWindowEx(NULL, L"Button", L"继续", WS_CHILD | WS_VISIBLE,
			360, 10, 80, 50, hwnd, (HMENU)BTN_RESUME, g_Instance, NULL);
		return 0;
	}

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case BTN_PLAY: {
			LRESULT len = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);//读取edit值
			WCHAR* str = NULL;
			str = (WCHAR*)malloc(len + 1);
			SendMessage(hEdit, WM_GETTEXT, len + 1, (LPARAM)str);
			pVoice->Speak(str, SPF_ASYNC, NULL);
			break;
		}
		case BTN_PAUSE:
			pVoice->Pause();
			break;
		case BTN_RESUME:
			pVoice->Resume();
			break;
		default:
			break;
		}
		return 0;
	}

	case WM_CLOSE:
		if (MessageBox(hwnd, L"确定退出吗?", L"Application", MB_OKCANCEL) == IDOK)
			DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		// 将WM_QUIT消息放到消息队列，它使GetMessage()函数返回0
		PostQuitMessage(0);
		return 0;
	}

	// 默认窗口处理程序
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
