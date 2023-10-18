#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("Code Convert");

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow){
	WNDCLASS wc = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,0,
		hInstance,
		NULL, LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		lpszClass
	};
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(
				lpszClass,
				lpszClass,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				NULL,
				(HMENU)NULL,
				hInstance,
				NULL
			);
	ShowWindow(hWnd, nCmdShow);

	MSG msg;
	while(GetMessage(&msg, nullptr, 0,0)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR *Mes = "마우스 좌우 버튼을 누르면 안시, 유니코드간을 변환합니다.";
	TCHAR *Mes2 = "UNICODE입니다.";
	wchar_t check[250];
	char* ansi = "안시 문자열을 유니코드로 변환합니다.";
	wchar_t atow[250];
	wchar_t* uni = L"유니코드 문자열을 안시로 변환합니다.";
	char wtoa[250];
	BOOL Uni;

	/*
		여기서 설명을 좀 추가해야할 것 같다.

		MultiByteToWideChar의 목적지는 결국 UTF-16이다.
		즉, 첫 번째 인수로 들어가는 CP_UTF8은 현재 이 파일이 어떻게 쓰여졌는가에 따라 달라질 수 있다.
		CP_ACP는 Windows에서 기본으로 사용되는 ANSI를 말하는데 한국어 완성형(949)를 따른다.

		내 컴퓨터에선 vim의 rc파일 설정을 utf-8로 지정해뒀기 때문에 현재 이 파일의 인코딩 상태는
		utf8이다.

		첫 번째 인수를 CP_ACP로 지정하면 dwFlags의 값을 아무리 바꿔도 변환되지 않는다.
		즉, 현재 코드 페이지는 65001(utf8)이며 MultiByteToWideChar 함수를 이용하여 UTF-16으로
		변환하는 것이다.

		반대로, WideCharToMultiByte의 목적지는 정확하지 않다.
		즉, 첫 번째 인수로 들어가는 CP_UTF8은 위 함수의 새로운 목적지가 된다.
		MultiByteToWideChar의 목적은 UTF-16이고 WideCharToMultiByte의 목적은 사용자가 지정해야만 한다.


		WideCharToMultiByte의 마지막 두 인수를 보면 알겠지만, 기존 WideChar(utf-16)에서
		utf8이나 ACP로 변환할 때 오류가 종종 생긴다.

		이를 위해 실패할 때 쓰이는 두 인수는 각각 순서대로 LPCSTR, LPBOOL 형을 가지며 대체할 문자열과
		BOOL 값을 대입하면 된다.
	 */

	switch(iMessage){
		case WM_LBUTTONDOWN:
			Uni = IsWindowUnicode(hWnd);
			MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, ansi,-1, atow,250);
			MessageBoxW(hWnd, atow, L"안시 -> 유니코드", MB_OK);
			return 0;

		case WM_RBUTTONDOWN:
			WideCharToMultiByte(CP_UTF8, 0, uni,-1, wtoa,250, NULL, NULL);
			MessageBoxA(hWnd, wtoa, "유니코드 -> 안시", MB_OK);
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 10,10, Mes, lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
