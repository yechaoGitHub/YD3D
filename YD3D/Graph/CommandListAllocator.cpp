#include "CommandListAllocator.h"

namespace YD3D 
{
	CommandListAllocator::CommandListAllocator()
	{
	}

	CommandListAllocator::~CommandListAllocator()
	{
	}

	bool CommandListAllocator::Create(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, uint32_t count)
	{
		mDevice = device;
		mCommandType = type;

		for (uint32_t i = 0; i < count; i++)
		{
			CommandPair pair;
			ThrowIfFailed(mDevice->CreateCommandAllocator(mCommandType, IID_PPV_ARGS(&pair.CommandAllocator)));
			ThrowIfFailed(mDevice->CreateCommandList(0, mCommandType, pair.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pair.CommandList)));
			pair.Free = true;

			mMapCommandList.insert(std::move(std::make_pair(pair.CommandList.Get(), std::move(pair))));
		}

		return true;
	}

	ID3D12GraphicsCommandList* CommandListAllocator::Allocate()
	{
		std::scoped_lock<std::mutex> al(mLock);

		for (auto& pair : mMapCommandList)
		{
			if (pair.second.Free)
			{
				pair.second.Free = false;
				return dynamic_cast<ID3D12GraphicsCommandList*>(pair.first);
			}
		}

		CommandPair pair;
		ThrowIfFailed(mDevice->CreateCommandAllocator(mCommandType, IID_PPV_ARGS(&pair.CommandAllocator)));
		ThrowIfFailed(mDevice->CreateCommandList(0, mCommandType, pair.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pair.CommandList)));
		pair.Free = false;

	
		mMapCommandList.insert(std::make_pair(pair.CommandList.Get(), pair));

		return pair.CommandList.Get();
	}

	bool CommandListAllocator::Reset(ID3D12GraphicsCommandList* commandList)
	{
		std::scoped_lock<std::mutex> al(mLock);
		if (mMapCommandList.find(commandList) != mMapCommandList.end()) 
		{
			auto& pair = mMapCommandList[commandList];
			ThrowIfFailed(pair.CommandAllocator->Reset());
			ThrowIfFailed(pair.CommandList->Reset(pair.CommandAllocator.Get(), nullptr));
			return true;
		}
		else 
		{
			return false;
		}
	}

	bool CommandListAllocator::Free(ID3D12GraphicsCommandList* commandList)
	{
		std::scoped_lock<std::mutex> al(mLock);
		if (mMapCommandList.find(commandList) != mMapCommandList.end())
		{
			auto& pair = mMapCommandList[commandList];
			ThrowIfFailed(pair.CommandAllocator->Reset());
			ThrowIfFailed(pair.CommandList->Reset(pair.CommandAllocator.Get(), nullptr));
			pair.Free = true;
			return true;
		}
		else 
		{
			return false;
		}
	}
}

