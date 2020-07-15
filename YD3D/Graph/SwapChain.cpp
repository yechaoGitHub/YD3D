#include "SwapChain.h"
#include "GraphicTask.h"

namespace YD3D
{
	SwapChain::SwapChain()
	{
	}

	SwapChain::~SwapChain()
	{
	}

	bool SwapChain::Create(IDXGIFactory5* factory, ID3D12Device* device, HWND winHandle, uint32_t width, uint32_t height)
	{
		mFactory = factory;
		mDevice = device;
		//mCommandQueue = commandQueue;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;


		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		CommandQueue& swapChainQueue = GraphicTask::GetGraphicTaskCommandQueue(ECommandQueueType::ESWAP_CHAIN);
		ThrowIfFailed(mFactory->CreateSwapChainForHwnd(swapChainQueue.Queue(), winHandle, &swapChainDesc, nullptr, nullptr, &swapChain1));
		ThrowIfFailed(swapChain1.As(&mSwapChain));

		for (UINT i = 0; i < 2; i++)
		{
			mRT[i].assign(new GraphicRenderTarget);
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(mRT[i]->GetResourceAddressOf())));
		}

		return true;
	}

	bool SwapChain::Present(UINT syncInterval, UINT presentFlags)
	{
		ThrowIfFailed(mSwapChain->Present(syncInterval, presentFlags));
		return true;
	}

	uint32_t SwapChain::GetBackBufferCount()
	{
		return 2;
	}

	GraphicRenderTarget* SwapChain::GetBackBuffer(uint32_t index)
	{
		return mRT[index].get_raw_ptr();
	}

	uint32_t SwapChain::GetCurBackBufferIndex()
	{
		return mSwapChain->GetCurrentBackBufferIndex();
	}

	GraphicRenderTarget* SwapChain::GetCurBackBuffer()
	{
		return mRT[mSwapChain->GetCurrentBackBufferIndex()].get_raw_ptr();
	}
};
