#include "YWindow.h"

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

	YWindow::YWindow() :
		mMouseCaptured(false),
		mMouseHold(false)
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

	uint32_t YWindow::GetWidth()
	{
		return mWidth;
	}

	uint32_t YWindow::GetHeight()
	{
		return mHeight;
	}

	//bool YWindow::MouseCaptured()
	//{
	//	return false;
	//}

	//EMouseKey YWindow::MouseHold()
	//{
	//	return EMouseKey();
	//}

	LRESULT YWindow::MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_ACTIVATE:
			OnActivate(static_cast<EActivateState>(wParam));
			break;
		
		case WM_LBUTTONDOWN:
			OnMouseDown(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_LBUTTONUP:
			OnMouseUp(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_RBUTTONDOWN:
			OnMouseDown(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		
		case WM_RBUTTONUP:
			OnMouseUp(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_MOUSEMOVE:
			OnMouseMove(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_NCMOUSEMOVE:
			OnNcMouseMove(static_cast<EMouseKey>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			break;

		case WM_KEYUP:
			OnKeyUp(wParam, lParam);
			break;

		case WM_SIZE:
			OnSize(static_cast<EResizeState>(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;

		}

		return DefWindowProc(mHwnd, msg, wParam, lParam);
	}

	void YWindow::OnMouseMove(EMouseKey btnState, uint32_t x, uint32_t y)
	{
	}

	void YWindow::OnMouseUp(EMouseKey btnState, uint32_t x, uint32_t y)
	{
	}

	void YWindow::OnMouseDown(EMouseKey btnState, uint32_t x, uint32_t y)
	{
	}

	void YWindow::OnKeyDown(VirtualKey key, VirtualKeyState keyState)
	{
	}

	void YWindow::OnKeyUp(VirtualKey key, VirtualKeyState keyState)
	{
	}

	void YWindow::OnActivate(EActivateState activate)
	{
	}

	void YWindow::OnSize(EResizeState state, uint32_t x, uint32_t y)
	{
	}

	void YWindow::OnNcMouseMove(EMouseKey btnState, uint32_t x, uint32_t y)
	{
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


