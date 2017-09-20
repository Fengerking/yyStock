// yyStock.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "yyStock.h"

#include "CIOcurl.h"
#include "CStockItemList.h"
#include "CPngDec.h"

#include "UStockParser.h"
#include "UStockTools.h"

#include "USystemFunc.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);


CPngDec pngDec;

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
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_YYSTOCK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

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

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_YYSTOCK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_YYSTOCK);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   pngDec.OpenSource("http://img1.money.126.net/chart/hs/time/540x360/1300316.png");
//	CIOcurl ioURL;
//	ioURL.Open("http://quotes.money.163.com/service/chddata.html?code=1300316&start=20170901&end=20170917", 0, 0);
//	ioURL.Open("http://api.money.126.net/data/feed/0601398", 0, 0);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
  
   int nStart = qcGetSysTime();

//   SetTimer(hWnd, 1001, 500, NULL);
     CStockItemList stockList;
	 stockList.OpenFileList();
	 
	 int nUsed = qcGetSysTime() - nStart;
	 nStart = qcGetSysTime();

//   qcStockRealTimeItem * pStockInfo = new qcStockRealTimeItem();
//   memset(pStockInfo, 0, sizeof(qcStockRealTimeItem));
//   qcStock_ParseRealTimeInfo("600895", pStockInfo);
//   delete pStockInfo;
/*
	 qcStockInfoItem * pItem = NULL;
	 NODEPOS pos = stockList.m_lstStock.GetHeadPosition();
	 while (pos != NULL)
	 {
		pItem = stockList.m_lstStock.GetNext(pos);
		qcStock_DownLoadHistoryData(pItem->m_szCode);
	 }
*/

//	 CObjectList<qcStockKXTInfoItem> lstHistory;
//	 qcStock_ParseHistoryData("000001", &lstHistory);
	 
	 nUsed = qcGetSysTime() - nStart;

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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_TIMER:
		pngDec.OpenSource("http://img1.money.126.net/chart/hs/time/540x360/0600895.png");
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hWnd, &ps);
		HBITMAP hBmp = pngDec.GetBmp();
		if (hBmp != NULL)
		{
			RECT rcView;
			GetClientRect(hWnd, &rcView);
			HDC hMemDC = CreateCompatibleDC(hdc);
			HBITMAP hOld = (HBITMAP)SelectObject(hMemDC, hBmp);
			//BitBlt(hdc, 0, 0, pngDec.GetWidth (), pngDec.GetHeight (), hMemDC, 0, 0, SRCCOPY);
			StretchBlt(hdc, 0, 0, rcView.right, rcView.bottom, hMemDC, 0, 0, pngDec.GetWidth(), pngDec.GetHeight(), SRCCOPY);
			SelectObject(hMemDC, hOld);
			DeleteDC(hMemDC);
		}

		EndPaint(hWnd, &ps);
	}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
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
