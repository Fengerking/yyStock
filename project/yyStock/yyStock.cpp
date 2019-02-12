// yyStock.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "yyStock.h"

#include "CIOcurl.h"
#include "CStockItemList.h"

#include "CWndGrpMng.h"

#include "UStockParser.h"
#include "UStockTools.h"

#include "USystemFunc.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE		g_hInst;								// current instance
TCHAR			g_szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR			g_szWindowClass[MAX_LOADSTRING];			// the main window class name
HBRUSH			g_hBrushBG = NULL;
CWndGrpMng *	g_pWndMng = NULL;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, g_szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_YYSTOCK, g_szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_YYSTOCK));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	g_hBrushBG = CreateSolidBrush(RGB(0, 0, 0));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YYSTOCK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = g_hBrushBG;// (HBRUSH)(COLOR_GRAYTEXT);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_YYSTOCK);
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	int	nScreenX = GetSystemMetrics(SM_CXSCREEN);
	int	nScreenY = GetSystemMetrics(SM_CYSCREEN);
	HWND hWnd;
	g_hInst = hInstance; // Store instance handle in our global variable
	hWnd = CreateWindow(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
						//nScreenX / 4, 200, nScreenX / 2, nScreenY / 2, NULL, NULL, hInstance, NULL);
						0, 0, nScreenX, nScreenY, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;

	CIOcurl ioHttp;
	//qcStock_DownLoadData_FHSP(&ioHttp, "000001");
	//qcStock_DownLoadData_Info(&ioHttp, "600895");
	//qcStock_DownLoadData_FHSP(&ioHttp, "600895");

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	g_pWndMng = new CWndGrpMng(g_hInst);
	g_pWndMng->CreateWnd(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			if (g_pWndMng != NULL)
				g_pWndMng->OnReceiveMessage(hWnd, message, wParam, lParam);
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_MSG_CHILDWND_MSG:
		if (g_pWndMng != NULL)
			return g_pWndMng->OnReceiveMessage(hWnd, message, wParam, lParam);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		if (g_pWndMng != NULL)
			delete g_pWndMng;
		g_pWndMng = NULL;
		PostQuitMessage(0);
		if (g_hBrushBG != NULL)
			DeleteObject(g_hBrushBG);
		break;

	default:
		if (g_pWndMng != NULL)
		{
			if (g_pWndMng->OnReceiveMessage(hWnd, message, wParam, lParam) == S_OK)
				return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
