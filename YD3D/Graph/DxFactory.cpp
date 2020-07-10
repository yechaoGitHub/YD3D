#include "DxFactory.h"

namespace YD3D
{
	using namespace Microsoft::WRL;

	DxFactory::DxFactory()
	{
	}

	DxFactory::~DxFactory()
	{
	}

	bool DxFactory::Create()
	{
#if defined(DEBUG) || defined(_DEBUG) 
		// Enable the D3D12 debug layer.
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
		}
#endif
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mFactory)));
		ThrowIfFailed(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice)));

		return true;
	}

	IDXGIFactory5* DxFactory::Factory()
	{
		return mFactory.Get();
	}

	ID3D12Device* DxFactory::Device()
	{
		return mDevice.Get();
	}

};

