#pragma once
#include <windows.h>
#include <Windowsx.h>

namespace YD3D
{
	enum class EMouseKey : uint64_t
	{
		EMK_CONTROL = 0x0008, 
		EMK_LBUTTON = 0x0001, 
		EMK_MBUTTON = 0x0010, 
		EMK_RBUTTON = 0x0002, 
		EMK_SHIFT = 0x0004, 
		EMK_XBUTTON1 = 0x0020, 
		EMK_XBUTTON2 = 0x0040
	};
	
	enum class EResizeState : uint64_t
	{
		ESIZE_MAXHIDE = 4,
		ESIZE_MAXIMIZED = 2,
		ESIZE_MAXSHOW = 3,
		ESIZE_MINIMIZED = 1,
		ESIZE_RESTORED = 0,
	};

	enum class EActivateState : uint64_t
	{
		EWA_ACTIVE = 1,
		EWA_CLICKACTIVE = 2,
		EWA_INACTIVE = 0,
	};

	typedef WPARAM VirtualKey;
	typedef LPARAM VirtualKeyState;

};
