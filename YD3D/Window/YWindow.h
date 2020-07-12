#pragma once
#include "YD3D_Header.h"

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

	protected:
		virtual LRESULT MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam);
		//virtual void OnSize();

	private:
		HWND			mHwnd;
		uint32_t		mWidth;
		uint32_t		mHeight;
		std::wstring	mCaption;

		static std::unordered_map<HWND, YWindow*> mMapWindow;
		static void AddMap(HWND hwnd, YWindow* window);
		static void RemoveMap(HWND hwnd);

		void InitWindow();

		friend LRESULT CALLBACK InterWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

};