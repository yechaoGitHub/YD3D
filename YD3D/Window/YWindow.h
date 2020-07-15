#pragma once
#include "YD3D_Header.h"
#include "YWndowDefine.h"

namespace YD3D
{
	class YWindow 
	{
	public:
		YWindow();
		virtual ~YWindow();

		virtual bool Create(uint32_t width, uint32_t height, const std::wstring &caption = L"");
		virtual uint32_t Run();
		HWND GetHandle();
		uint32_t GetWidth();
		uint32_t GetHeight();

		/*bool MouseCaptured();
		EMouseKey MouseHold();*/

	protected:
		virtual LRESULT MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam);
		virtual void OnMouseMove(EMouseKey btnState, uint32_t x, uint32_t y);
		virtual void OnMouseUp(EMouseKey btnState, uint32_t x, uint32_t y);
		virtual void OnMouseDown(EMouseKey btnState, uint32_t x, uint32_t y);
		virtual void OnKeyDown(VirtualKey key, VirtualKeyState keyState);
		virtual void OnKeyUp(VirtualKey key, VirtualKeyState keyState);
		virtual void OnActivate(EActivateState activate);
		virtual void OnSize(EResizeState state, uint32_t x, uint32_t y);
		virtual void OnNcMouseMove(EMouseKey btnState, uint32_t x, uint32_t y);

	private:
		HWND			mHwnd;
		uint32_t		mWidth;
		uint32_t		mHeight;
		std::wstring	mCaption;
		bool			mMouseCaptured;
		bool			mMouseHold;

		static std::unordered_map<HWND, YWindow*> mMapWindow;
		static void AddMap(HWND hwnd, YWindow* window);
		static void RemoveMap(HWND hwnd);

		void InitWindow();

		friend LRESULT CALLBACK InterWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

};