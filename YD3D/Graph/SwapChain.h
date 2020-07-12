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

		bool Create(IDXGIFactory5* factory, ID3D12Device* device, HWND winHandle, uint32_t width, uint32_t height);
		bool Present(UINT syncInterval, UINT presentFlags);
		uint32_t GetBackBufferCount();
		GraphicRenderTarget* GetBackBuffer(uint32_t index);
		uint32_t GetCurBackBufferIndex();
		GraphicRenderTarget* GetCurBackBuffer();
		
	private:
		IDXGIFactory5*								mFactory;
		ID3D12Device*								mDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain3>		mSwapChain;
		gc_ptr<GraphicRenderTarget>					mRT[2];
	};
};

