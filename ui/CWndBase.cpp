/*******************************************************************************
	File:		CWndBase.cpp

	Contains:	Window base implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndBase.h"

LRESULT CALLBACK CWndBase::ViewWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CWndBase * pViewWindow = (CWndBase *)GetWindowLong (hWnd, GWL_USERDATA);
	if (pViewWindow == NULL)
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	else
		return pViewWindow->OnReceiveMessage(hWnd,uMsg,wParam,lParam);
}

CWndBase::CWndBase(HINSTANCE hInst)
	: m_hInst (hInst)
	, m_hWnd (NULL)
	, m_hParent (NULL)
	, m_pGroup(NULL)
	, m_hBKBrush (NULL)
	, m_nClrFont (RGB(255,255,255))
	, m_nTimerUpdate (0)
	, m_nTimerFirst (0)
	, m_bLButtonClick (false)
	, m_nTimerLBClick (0)
	, m_nUpdateTime(1000)
	, m_pThreadWork(NULL)
{
	SetObjectName("CWndBase");

	_tcscpy (m_szClassName, _T("bangViewWidnow"));
	_tcscpy (m_szWindowName, _T("bangViewWidnow"));

	_tcscpy (m_szText, _T(""));
}

CWndBase::~CWndBase(void)
{
	if (m_pThreadWork != NULL)
	{
		m_pThreadWork->Stop();
		delete m_pThreadWork;
	}

	if (m_hWnd != NULL)
		Close ();
}

bool CWndBase::CreateWnd(HWND hParent, RECT rcView, COLORREF clrBG, CGroupBase * pGroup)
{
	m_hParent = hParent;
	m_pGroup = pGroup;
	if (m_hWnd != NULL)
	{
		SetParent (m_hWnd, hParent);
		return true;
	}
	HINSTANCE hInst = (HINSTANCE )GetWindowLong (hParent, GWL_HINSTANCE);
	WNDCLASS wcex;
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ViewWindowProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInst;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)::CreateSolidBrush (clrBG);
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= m_szClassName;

	RegisterClass(&wcex);

	if (m_hParent != NULL)
		m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParent, NULL, hInst, NULL);
	else
		m_hWnd = CreateWindow(m_szClassName, m_szWindowName, WS_VISIBLE | WS_BORDER | WS_POPUP,
								rcView.left, rcView.top, rcView.right, rcView.bottom, hParent, NULL, hInst, NULL);

	if (m_hWnd == NULL)
		return false;

	if (rcView.bottom == 0 || rcView.right == 0)
	{
		if (hParent != NULL)
			GetClientRect (hParent, &rcView);
	}

	SetWindowPos (m_hWnd, NULL, rcView.left, rcView.top, rcView.right - rcView.left, rcView.bottom - rcView.top, 0);
	LONG lRC = SetWindowLong (m_hWnd, GWL_USERDATA, (LONG)this);

	m_hBKBrush = CreateSolidBrush (clrBG);
	GetClientRect (m_hWnd, &m_rcWnd);

	return true;
}

void CWndBase::Close (void)
{
	SendMessage (m_hWnd, WM_CLOSE, 0, 0);
	m_hWnd = NULL;
}

void CWndBase::SetBGColor (COLORREF nColor)
{
	if (m_hBKBrush != NULL)
		DeleteObject (m_hBKBrush);

	m_hBKBrush = CreateSolidBrush (nColor);
	::InvalidateRect (m_hWnd, NULL, TRUE);
}

void CWndBase::SetText (TCHAR * pText)
{
	if (pText == NULL)
		return;

	_tcscpy (m_szText, pText);
	::InvalidateRect (m_hWnd, NULL, TRUE);
}

LRESULT CWndBase::OnResize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnKeyUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnMouseDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnMouseUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

LRESULT CWndBase::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return S_FALSE;
}

int	CWndBase::UpdateView(HDC hDC)
{
	return QC_ERR_NONE;
}

int	CWndBase::UpdateInfo(void)
{
	return QC_ERR_NONE;
}

int	CWndBase::ThreadStart(void)
{
	if (m_pThreadWork == NULL)
	{
		m_pThreadWork = new CThreadWork();
		m_pThreadWork->SetOwner(m_szObjName);
		m_pThreadWork->Start();
	}

	CThreadEvent * pEvent = new CThreadEvent(QC_THREAD_EVENT_UPDATE, 0, 0, NULL);
	pEvent->SetEventFunc(this, &CThreadFunc::OnEvent);
	m_pThreadWork->PostEvent(pEvent, 10);

	return QC_ERR_NONE;
}

int	CWndBase::ThreadStop(void)
{
	if (m_pThreadWork != NULL)
	{
		m_pThreadWork->Stop();
	}
	return QC_ERR_NONE;
}

int	CWndBase::OnHandleEvent(CThreadEvent * pEvent)
{
	UpdateInfo();

	CThreadEvent * pNewEvent = m_pThreadWork->GetFree();
	if (pNewEvent == NULL)
	{
		pNewEvent = new CThreadEvent(QC_THREAD_EVENT_UPDATE, 0, 0, NULL);
		pNewEvent->SetEventFunc(this, &CThreadFunc::OnEvent);
	}
	else
	{
		pNewEvent->m_nID = QC_THREAD_EVENT_UPDATE;
	}
	m_pThreadWork->PostEvent(pNewEvent, m_nUpdateTime);

	return QC_ERR_NONE;
}

LRESULT CWndBase::OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		PostMessage (m_hParent, uMsg, wParam, lParam);
		if (m_bLButtonClick)
		{
			PostMessage (m_hWnd, WM_LBUTTONDBLCLK, wParam, lParam);
			m_bLButtonClick = false;
		}
		else
		{
			m_bLButtonClick = true;
			m_nTimerLBClick = SetTimer (m_hWnd, WM_TIMER_LBUTTONCLICK, 300, NULL);
		}
		break;

	case WM_MOUSEMOVE:
		PostMessage (m_hParent, uMsg, wParam, lParam);
		break;

	case WM_TIMER:
		if (wParam == WM_TIMER_LBUTTONCLICK)
		{
			KillTimer (m_hWnd, m_nTimerLBClick);
			m_nTimerLBClick = 0;
			if (m_bLButtonClick)
				m_bLButtonClick = false;
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (_tcslen (m_szText) > 0)
		{
			RECT rcView;
			GetClientRect (m_hWnd, &rcView);
			SetTextColor (hdc, m_nClrFont);
			::SetBkMode (hdc, TRANSPARENT);
			DrawText (hdc, m_szText, _tcslen (m_szText), &rcView, DT_CENTER | DT_VCENTER);
		}

		EndPaint(hWnd, &ps);
	}
		break;

	case WM_ERASEBKGND:
	{
		HDC hdc = (HDC)wParam;
		RECT rcView;
		GetClientRect (m_hWnd, &rcView);
		FillRect (hdc, &rcView, m_hBKBrush);
		return S_OK;
	}
		break;

	case WM_DESTROY:
		DeleteObject (m_hBKBrush);
		UnregisterClass (m_szClassName, NULL);
		break;

	default:
		break;
	}

	return	DefWindowProc(hWnd, uMsg, wParam, lParam);
}


