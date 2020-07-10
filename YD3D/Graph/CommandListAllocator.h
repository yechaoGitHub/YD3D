#pragma once
#include "YD3D_Header.h"

namespace YD3D 
{
	struct CommandPair
	{
		CommandPair() :
			Free(false)
		{

		}

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		CommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	CommandList;
		bool												Free;
	};

	class CommandListAllocator 
	{
	public:
		CommandListAllocator();
		~CommandListAllocator();

		bool Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, uint32_t count);
		ID3D12GraphicsCommandList* Allocate();
		void Free(ID3D12GraphicsCommandList* commandList);

	private:
		ID3D12Device* mDevice;
		D3D12_COMMAND_LIST_TYPE														mCommandType;
		std::unordered_map<ID3D12GraphicsCommandList*, CommandPair>					mMapCommandList;
		std::mutex																	mLock;

	};

}
