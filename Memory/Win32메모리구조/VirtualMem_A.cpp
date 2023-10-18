#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("VMemA");

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
	TCHAR Mes[256];
	static TCHAR *str;
	HWND hB;

	/*
		페이지 테이블에 의해 관리되는 가상 메모리 공간에 대하여 알아보기 위한 실험이다.

		응용 프로그램의 주소와 가상 메모리의 주소가 각각 분리된 공간에 존재한다는 것을 알아보기 위해
		메세지를 이용한 통신 프로그램을 작성했다.

		응용 프로그램이 가지는 주소는 우연하게도 동일할 수 있으나, 이를 관리하는 페이지 테이블에 의해
		각기 다른 프로세스 즉, 가상메모리 공간을 사용한다는 것을 간단하게 입증해보자. 
	 */

	/*
		실험 결과 lParam으로 전달된 번지를 메시지 박스로 확인했다.

		그러나, 이 번지의 문자열을 TextOut으로 출력해 보면 엉뚱한 쓰레기 문자열만 출력된다.
		사용자 정의 메시지를 통해 포인터를 전달받기는 했지만 이 포인터가 가리키는 곳에는
		"This is a string"이라는 문자열이 들어있지 않다.

		왜냐하면 두 프로세스의 주소 공간이 완전히 격리되어 있기 때문에 똑같은 번지라도
		실제 참조하는 물리적인 메모리는 다르기 때문이다.

		두 프로세스가 같은 주소 공간에서 실행된다면 VMemB가 전달받은 번지에는 VMemA가
		복사해 놓은 문자열이 들어있어야 하겠지만 주소 공간이 틀리기 때문에 포인터를 전달하는 방식으로
		프로세스끼리 문자열을 교환할 수 없다.


		분리된 주소 공간에서 포인터는 각 프로세스에 대해 지역적이다.
		만약 VMemB가 전달받은 포인터 영역이 가상 메모리에 맵핑되어 있지 않다거나 사용 금지 영역이라면
		VMemB는 액세스 위반으로 죽을 수도 있다.
	*/

	/*
		이런 주소 공간의 분리에 의해 프로세스간 악영향을 미칠 가능성이 전혀 없다.

		설사 A가 심각한 버그를 가지고 있어 임의의 번지를 마구 액세스한다고 하더라도
		혼자 다운될 뿐 다른 응용 프로그램이나 운영체제 자체를 손상시킬 수는 없다.
	 */

	switch(iMessage){
		case WM_CREATE:
			str = (TCHAR*)malloc(128);
			lstrcpy(str, TEXT("This is a string"));
			return 0;

		case WM_LBUTTONDOWN:
			hB = FindWindow(TEXT("VMemB"), NULL);
			if(hB){
				PostMessage(hB, WM_USER+1, NULL, (LPARAM)str);
			}
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			wsprintf(Mes, TEXT("왼쪽 버튼을 누르면 %x번지의 문자열을 VMemB로 보냅니다."), str);
			TextOut(hdc, 10,10, Mes, lstrlen(Mes));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
