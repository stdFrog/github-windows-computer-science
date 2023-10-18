#define UNICODE
#include <windows.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = TEXT("BigMemory2 Used malloc");

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
		free(stptr);
		stptr=NULL;
	}
	wsprintf(Status, TEXT("할당되지 않았습니다."));
	InvalidateRect(hWndMain, NULL, TRUE);
}



// 이번 문제는 포인터가 아닌 일반 변수가 세 개 필요하다.
// realloc에 의해 베이스가 바뀔 수 있으므로 포인터는 안된다.

// stptr: 할당된 메모리 공간의 시작 주소
// AllocSize: 현재 할당된 크기
// RemainSize: 할당된 공간중 남은 공간
void ReadRecords(){
	int i,c;
	int RecSize;
	int AllocSize;
	int RemainSize;

	FreeRecords();

	// 최초 2M를 할당한다.
	AllocSize = RemainSize = 2*MEGA;
	stptr = (PBYTE)malloc(AllocSize);
	if(stptr == NULL){
		MessageBox(hWndMain, TEXT("메모리 할당 실패"), TEXT("에러"), MB_OK);
		return;
	}
	
	// 최대 100개의 레코드를 읽어들인다.
	c = rand()%91+10;
	for(i=0; i<c; i++){
		if(RemainSize < MEGA){
			AllocSize += MEGA;
			RemainSize += MEGA;
			stptr = (PBYTE)realloc(stptr, AllocSize);
		}

		// 10K ~ 1M 사이의 레코드를 읽어들인다.
		RecSize = ((rand()%100)+1) * 10240;
		memset(stptr+AllocSize-RemainSize, i, RecSize);
		RemainSize -= RecSize;
	}

	wsprintf(Status, TEXT("총 할당량: %d 메가, 사용: %d 메가"), AllocSize/MEGA, (AllocSize-RemainSize)/MEGA);
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
