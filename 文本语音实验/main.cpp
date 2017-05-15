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

// ���崰�ڵ���Ϊ��hwndΪ���ھ����uMsgΪ��Ϣ���룬wParam��lParamΪ��Ϣ�Ķ�������
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Ӧ�ó�����ʼ�㡣hInstanceΪӦ�ó�������pCmdLine�����в�����nCmdShow�����ڱ�־����󻯣���С������ʾ�ȣ�
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	g_Instance = hInstance;

	if (FAILED(CoInitialize(NULL)))
		return -1;

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);//ע��pvoice
	if (FAILED(hr)) {
		return -1;
	}
	else if (SUCCEEDED(hr)) {
		//������󶨵��ļ�����
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

	// ע�ᴰ����
	// �����ඨ�崰�ڵĹ�����Ϊ��ÿ���������е����ݽ���ʵ�����ݣ�Instance Data��
	// ÿ�����ڱ������һ��������
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc; // ָ�򴰿ڴ�������ָ��
	wc.hInstance = hInstance; // Ӧ�ó���ʵ���ľ��
	wc.lpszClassName = CLASS_NAME; // ��־��������ַ���
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	RegisterClass(&wc);

	// ��������
	hwnd = CreateWindowEx(
		WS_EX_ACCEPTFILES, // Optional window styles.
		CLASS_NAME, // �����������
		L"�ı��ʶ�����", // �����Ǵ��ڵı���
		WS_OVERLAPPEDWINDOW, // ������۷��

		CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, // ���ڵ�λ�úʹ�С������ʹ��Ĭ��ֵ

		NULL, // �����ڣ�parant window����ӵ�д��ڣ�owner window��
		NULL, // �˵���Menu��
		hInstance, // Ӧ�ó���ʵ���ľ��
		NULL // ��������ݣ�void*����
	);

	if (hwnd == NULL) {
		return 0;
	}

	// ��ʾ������
	ShowWindow(hwnd, nCmdShow);

	// ��Ϣѭ��
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) // ����Ϣ���л�ȡ��Ϣ
	{
		TranslateMessage(&msg); // ת����������
		DispatchMessage(&msg); // �����øú���ʱ�����ڼ�ӵ���WindowProc����
	}

	CoUninitialize();

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
		hEdit = CreateWindowEx(NULL, L"Edit", L"������Ҫ��������", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOHSCROLL | ES_MULTILINE,
			50, 70, 500, 400, hwnd, NULL, g_Instance, NULL);
		hPlayBtn = CreateWindowEx(NULL, L"Button", L"����", WS_CHILD | WS_VISIBLE,
			160, 10, 80, 50, hwnd, (HMENU)BTN_PLAY, g_Instance, NULL);
		hPauseBtn = CreateWindowEx(NULL, L"Button", L"��ͣ", WS_CHILD | WS_VISIBLE,
			260, 10, 80, 50, hwnd, (HMENU)BTN_PAUSE, g_Instance, NULL);
		hResumeBtn = CreateWindowEx(NULL, L"Button", L"����", WS_CHILD | WS_VISIBLE,
			360, 10, 80, 50, hwnd, (HMENU)BTN_RESUME, g_Instance, NULL);
		return 0;
	}

	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case BTN_PLAY: {
			LRESULT len = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);//��ȡeditֵ
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
		if (MessageBox(hwnd, L"ȷ���˳���?", L"Application", MB_OKCANCEL) == IDOK)
			DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		// ��WM_QUIT��Ϣ�ŵ���Ϣ���У���ʹGetMessage()��������0
		PostQuitMessage(0);
		return 0;
	}

	// Ĭ�ϴ��ڴ������
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
