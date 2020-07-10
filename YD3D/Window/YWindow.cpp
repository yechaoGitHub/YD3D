#include "YWindow.h"
#include <windows.h>

namespace YD3D
{
	std::unordered_map<HWND, YWindow*> YWindow::mMapWindow;

	LRESULT CALLBACK InterWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		switch (msg)
		{
		case WM_NCCREATE: 
		{
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			YWindow* window = reinterpret_cast<YWindow*>(pCreateStruct->lpCreateParams);
			YWindow::AddMap(hwnd, window);
		}
		case WM_CREATE: 
		case WM_GETMINMAXINFO:
		case WM_NCCALCSIZE:
		return DefWindowProc(hwnd, msg, wParam, lParam);

		default:
			return YWindow::mMapWindow[hwnd]->MainWndProc(msg, wParam, lParam);
		}

		return ERROR;
	}

	void YWindow::AddMap(HWND hwnd, YWindow* window)
	{
		assert(mMapWindow.insert(std::make_pair(hwnd, window)).second);
	}

	void YWindow::RemoveMap(HWND hwnd)
	{
		assert(mMapWindow.erase(hwnd));
	}

	YWindow::YWindow()
	{

	}

	YWindow::~YWindow()
	{
		RemoveMap(mHwnd);
		DestroyWindow(mHwnd);
	}

	bool YWindow::Create(uint32_t width, uint32_t height, const std::wstring& caption)
	{
		mWidth = width;
		mHeight = height;
		mCaption = caption;
		InitWindow();
		
		return true;
	}

	uint32_t YWindow::Run()
	{
		MSG msg = { 0 };

		while (msg.message != WM_QUIT)
		{
			// If there are Window messages then process them.
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		return (int)msg.wParam;
	}

	HWND YWindow::GetHandle()
	{
		return mHwnd;
	}

	LRESULT YWindow::MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//switch (msg)
		//{
		//case WM_ACTIVATE:
		//	if (LOWORD(wParam) == WA_INACTIVE)
		//	{
		//		
		//	}
		//	else
		//	{
		//		
		//	}
		//	return 0;

		//case WM_SIZE:

		//	return 0;

		//case WM_ENTERSIZEMOVE:
		//	return 0;

		//	
		//case WM_EXITSIZEMOVE:
		//	return 0;


		//case WM_DESTROY:
		//	PostQuitMessage(0);
		//	return 0;

		//case WM_MENUCHAR:
		//	return MAKELRESULT(0, MNC_CLOSE);

		//case WM_GETMINMAXINFO:
		//	((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		//	((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		//	return 0;

		//case WM_LBUTTONDOWN:
		//case WM_MBUTTONDOWN:
		//case WM_RBUTTONDOWN:
		//	
		//	return 0;
		//case WM_LBUTTONUP:
		//case WM_MBUTTONUP:
		//case WM_RBUTTONUP:
		//	
		//	return 0;
		//case WM_MOUSEMOVE:
		//	
		//	return 0;
		//case WM_KEYUP:
		//	if (wParam == VK_ESCAPE)
		//	{
		//		PostQuitMessage(0);
		//	}
		//	return 0;
		//}

		return DefWindowProc(mHwnd, msg, wParam, lParam);
	}

	void YWindow::InitWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = InterWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = nullptr;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		assert(RegisterClass(&wc));
		
		RECT R = { 0, 0, mWidth, mHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		mHwnd = CreateWindow(L"MainWnd", mCaption.c_str(),
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, nullptr, this);

		assert(mHwnd);

		ShowWindow(mHwnd, SW_SHOW);
		UpdateWindow(mHwnd);
	}
};


