#include <windows.h>
#include <crtdbg.h>


HWND g_hWnd = NULL;
LARGE_INTEGER g_cntTimer = { 0 };//時間計測用カウンター

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// wWinMain 文字列をユニコードにする場合のメイン関数
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);//引数未使用のWarnig対策
	UNREFERENCED_PARAMETER(lpCmdLine);

	//メモリリークデバッグ
#ifdef _DEBUG	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//ウインドウ作成
	if (FAILED(InitWindow(hInstance, nCmdShow))) {
		return 0;//作成失敗
	}

	//プログラムの初期化など


	// 時間計測用カウンター
	LARGE_INTEGER cntfreq;
	QueryPerformanceFrequency(&cntfreq);//カウンターの周波数
	QueryPerformanceCounter(&g_cntTimer);//カウンター取得

										 // メッセージループ
	MSG msg = { 0 };
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			//繰り返し呼び出されるメイン処理


			//60FPS固定
			const float fps = 60.0f;
			while (1) {
				LARGE_INTEGER count;
				QueryPerformanceCounter(&count);
				long long cnttime = count.QuadPart - g_cntTimer.QuadPart;//経過時間(カウント）
				float time = float(double(cnttime) / double(cntfreq.QuadPart));//経過時間(秒）
				if (time >= 1.0f / fps) {
					g_cntTimer = count;
					break;
				}
				else {
					// 長時間待つ場合CPUを占有しないようにする
					float wait_time = 1.0f / fps - time;
					if (wait_time > 0.002f) {
						int wait_milli = static_cast<int>(wait_time*1000.0f);
						Sleep(DWORD(wait_milli - 1));
						//待ち時間が1ミリ秒以内になるまで待つ
					}
				}
			}
		}
	}

	//終了処理

	return 1;
}

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "VS2017Win32";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex)) {
		return E_FAIL;
	}
	//ウインドウのクライアント領域（=DirectXの描画領域）を指定
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindow("VS2017Win32", "VisualStudio2017 Win32", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL);
	if (!g_hWnd) {
		return E_FAIL;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	return S_OK;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

