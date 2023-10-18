#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("BigMemory");

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

#define MEGA 1048576
PBYTE stptr = NULL;
TCHAR Status[256] = TEXT("할당되지 않았습니다.");

HWND hWndMain;

void FreeRecords(){
	// 할당되어 있던 메모리를 해제한다.
	if(stptr!=NULL){
		if(VirtualFree(stptr, 100*MEGA, MEM_DECOMMIT) == 0){
			MessageBox(hWndMain, TEXT("메모리 확장 해제 실패"), TEXT("에러"), MB_OK);
		}
		if(VirtualFree(stptr, 0, MEM_RELEASE) == 0){
			MessageBox(hWndMain, TEXT("메모리 예약 해제 실패"), TEXT("에러"), MB_OK);
		}
		stptr = NULL;
	}
	wsprintf(Status, TEXT("할당되지 않았습니다."));
	InvalidateRect(hWndMain, NULL, TRUE);
}



// 세 개의 포인터 변수가 필요하다.
// stptr: 최초 할당된 메모리 공간의 시작 주소
// endptr: 현재까지 확정된 메모리 공간의 끝 주소
// nowptr: 현재 사용중인 메모리의 주소

void ReadRecords(){
	int i,c;
	int RecSize;
	PBYTE nowptr, endptr;

	FreeRecords();

	// 100M의 주소 공간을 예약한다.
	stptr = (PBYTE)VirtualAlloc(NULL, 100*MEGA, MEM_RESERVE, PAGE_READWRITE);

	if(stptr == NULL){
		MessageBox(hWndMain, TEXT("메모리 예약 실패"), TEXT("에러"), MB_OK);
		return;
	}

	nowptr = stptr;
	endptr = stptr;

	// 최대 100개의 레코드를 읽어 들인다.
	c = rand()%90+10;
	for(i=0; i<c; i++){
		// 확정된 메모리가 1M 미만이면 1M를 더 확장한다.
		if(endptr - nowptr < MEGA){
			VirtualAlloc(endptr, MEGA, MEM_COMMIT, PAGE_READWRITE);
			endptr+= MEGA;
		}

		// 10K - 1M 사이의 레코드를 읽어들인다.
		RecSize = ((rand() % 100) + 1) * 10240;
		memset(nowptr, i, RecSize);
		nowptr+= RecSize;
	}

	wsprintf(Status, TEXT("예약: 100메가, 확정: %d메가, 사용: %d메가"), (endptr-stptr)/MEGA, (nowptr-stptr)/MEGA);
	InvalidateRect(hWndMain, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR* Mes = TEXT("왼쪽 마우스 버튼: 메모리 할당, 오른쪽 마우스 버튼: 메모리 해제");

	switch(iMessage){
		case WM_CREATE:
			hWndMain = hWnd;
			srand(GetTickCount());
			return 0;

		case WM_LBUTTONDOWN:
			SetCursor(LoadCursor(NULL, IDC_WAIT));
			ReadRecords();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return 0;

		case WM_RBUTTONDOWN:
			FreeRecords();
			return 0;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			TextOut(hdc, 50,50, Mes,lstrlen(Mes));
			TextOut(hdc, 50,80, Status, lstrlen(Status));
			EndPaint(hWnd, &ps);
			return 0;

		case WM_DESTROY:
			FreeRecords();
			PostQuitMessage(0);
			return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}
