#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicRenderTarget.h"

namespace YD3D
{
	class SwapChain 
	{
	public:
		SwapChain();
		~SwapChain();

		bool Create(IDXGIFactory5* factory, ID3D12Device* device, HWND winHandle, UINT backBufferCount, UINT width, UINT height);
		bool Present(UINT syncInterval, UINT presentFlags);
		uint32_t GetCurBackBufferIndex();
		GraphicRenderTarget& GetCurBackBuffer();

	private:
		IDXGIFactory5*								mFactory;
		ID3D12Device*								mDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>		mSwapChain;
		GraphicRenderTarget							mRT[2];
	};


};

