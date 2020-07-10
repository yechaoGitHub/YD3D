#include "Graph/DescriptorHeap.h"

namespace YD3D
{
	


	DescriptorHeap::DescriptorHeap()
	{
	}

	DescriptorHeap::~DescriptorHeap()
	{
	}

	bool DescriptorHeap::Create(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		assert(!mDescriptorHeap);
		mDevice = device;

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = heapType;
		desc.NumDescriptors = numDescriptors;
		desc.Flags = flags;

		

		device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mDescriptorHeap));
		mDescriptorCount = numDescriptors;
		mIncreasement = device->GetDescriptorHandleIncrementSize(heapType);

		return true;
	}

	ID3D12DescriptorHeap* DescriptorHeap::Descriptor() const
	{
		return mDescriptorHeap;
	}

	uint32_t DescriptorHeap::GetHandleIncreasement()
	{
		assert(mDescriptorHeap);
		return mIncreasement;
	}

	D3D12_DESCRIPTOR_HEAP_DESC DescriptorHeap::GetDesc()
	{
		assert(mDescriptorHeap);
		return std::move(mDescriptorHeap->GetDesc());
	}

	D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetType()
	{
		assert(mDescriptorHeap);
		D3D12_DESCRIPTOR_HEAP_DESC desc = GetDesc();
		return desc.Type;
	}

	ID3D12Device* DescriptorHeap::GetDevice()
	{
		assert(mDescriptorHeap);
		return mDevice;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandle(uint32_t Index)
	{
		assert(mDescriptorHeap);

		D3D12_CPU_DESCRIPTOR_HANDLE handle = {};
		handle = mDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += Index * GetHandleIncreasement();

		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandle(uint32_t Index)
	{
		assert(mDescriptorHeap);

		D3D12_GPU_DESCRIPTOR_HANDLE handle = {};
		handle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += Index * GetHandleIncreasement();

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetResourceCpuHandle(const GraphicResource& res)
	{
		assert(mDescriptorHeap);

		for (auto& pair : mMapResource)
		{
			auto& index = pair.first;
			auto& bindRes = pair.second;
			if (res.Resource() == bindRes->Resource())
			{
				return GetCpuHandle(index);
			}
		}

		return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetResourceGpuHandle(const GraphicResource& res)
	{
		assert(mDescriptorHeap);

		for (auto& pair : mMapResource)
		{
			auto& index = pair.first;
			auto& bindRes = pair.second;
			if (res.Resource() == bindRes->Resource())
			{
				return GetGpuHandle(index);
			}
		}

		return D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
	}

	bool DescriptorHeap::RemoveView(const GraphicResource& res)
	{
		assert(mDescriptorHeap);

		std::vector<uint32_t> removeKeys;
		for (auto& pair : mMapResource)
		{
			auto& compareRes = pair.second;
			if (res.Resource() == compareRes->Resource())
			{
				removeKeys.push_back(pair.first);
			}
		}

		for (auto& key : removeKeys)
		{
			mMapResource.erase(key);
		}

		return !removeKeys.empty();
	}

	GraphicResource* DescriptorHeap::GetSlot(uint32_t index)
	{
		assert(mDescriptorHeap);

		bool occupied(false);
		if (IsOccupied(index, 1, &occupied) && occupied)
		{
			return mMapResource[index];
		}

		return nullptr;
	}

	bool DescriptorHeap::IsOccupied(uint32_t start, uint32_t count, bool* isOccupied)
	{
		assert(mDescriptorHeap);

		for (UINT i = 0; i < count; i++)
		{
			UINT pos = start + i;
			if (pos >= mDescriptorCount)
			{
				return false;
			}

			if (mMapResource.find(pos) != mMapResource.end())
			{
				*isOccupied = true;
			}
		}

		*isOccupied = false;
		return true;
	}

	bool DescriptorHeap::FindFreeSlot(uint32_t count, uint32_t* slotIndex)
	{
		assert(mDescriptorHeap);

		UINT FreeSlot(0);
		UINT curSlot(0);
		for (auto& pair : mMapResource)
		{
			UINT interval = pair.first - curSlot;
			if (interval >= count)
			{
				*slotIndex = curSlot;
				return true;
			}
			else
			{
				curSlot = pair.first + 1;
			}
		}

		if (mDescriptorCount - curSlot >= count)
		{
			*slotIndex = curSlot;
			return true;
		}
		else
		{
			return false;
		}

		return false;
	}

	void DescriptorHeap::SetSlot(uint32_t index, GraphicResource* res)
	{
		mMapResource[index] = res;
	}
};