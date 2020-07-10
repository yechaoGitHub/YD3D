#pragma once
#include "YD3D_Header.h"

namespace YD3D
{
	class DxFactory 
	{
	public:
		DxFactory();
		~DxFactory();

		bool Create();

		IDXGIFactory5* Factory();
		ID3D12Device* Device();

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory5>		mFactory;
		Microsoft::WRL::ComPtr<ID3D12Device>		mDevice;
	};
};