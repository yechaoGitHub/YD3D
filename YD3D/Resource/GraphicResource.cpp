#include "GraphicResource.h"
#include "Graph/DescriptorHeapManager.h"

namespace YD3D
{
	GraphicResource::GraphicResource() :
		mResource(nullptr),
		mVecGcDescriptor(get_gc_allocator<gc_ptr<DescriptorHeap>>())
	{
	}

	GraphicResource::~GraphicResource()
	{
		if (mResource) 
		{
			Release();
		}
	}

	bool GraphicResource::Create(ID3D12Device* device, D3D12_HEAP_PROPERTIES* heapProperty, D3D12_HEAP_FLAGS flag, D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clearValue)
	{
		assert(!mResource);
		ThrowIfFailed(device->CreateCommittedResource(heapProperty, flag, desc, state, clearValue, IID_PPV_ARGS(&mResource)));
		return true;
	}

	void GraphicResource::Release()
	{
		assert(mResource);
		mResource->Release();
		mResource = nullptr;

		for (auto &vecDescHeapHandle : mBindDescriptor)
		{
			for (auto &handle : vecDescHeapHandle)
			{
				handle.DescriptorHeap->RemoveView(this);
			}
			vecDescHeapHandle.clear();
		}
	}

	ID3D12Resource* GraphicResource::Resource() const
	{
		return mResource;
	}

	ID3D12Device* GraphicResource::GetDevice() const
	{
		assert(mResource);
		ID3D12Device* ret(nullptr);
		ThrowIfFailed(mResource->GetDevice(IID_PPV_ARGS(&ret)));
		return ret;
	}

	ID3D12Resource** GraphicResource::GetResourceAddressOf()
	{
		return &mResource;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GraphicResource::GetGpuAddress() const
	{
		assert(mResource);
		return mResource->GetGPUVirtualAddress();
	}

	D3D12_RESOURCE_DESC GraphicResource::GetDesc() const
	{
		assert(mResource);
		return mResource->GetDesc();
	}

	D3D12_HEAP_PROPERTIES GraphicResource::GetHeapProperties() const
	{
		assert(mResource);
		D3D12_HEAP_PROPERTIES	heapProperties;
		D3D12_HEAP_FLAGS		flag;
		ThrowIfFailed(mResource->GetHeapProperties(&heapProperties, &flag));
		return heapProperties;
	}

	void GraphicResource::GetFootPrint(CopyableFootPrint& footPrint, uint64_t offset) const
	{
		assert(mResource);

		CopyableFootPrint& ret = footPrint;
		D3D12_RESOURCE_DESC desc = GetDesc();
		ID3D12Device* dev = GetDevice();

		UINT subresourceNum = desc.DepthOrArraySize * desc.MipLevels;
		ret.footPrints.assign(subresourceNum, D3D12_PLACED_SUBRESOURCE_FOOTPRINT());

		dev->GetCopyableFootprints(&desc, 0, subresourceNum, offset, ret.footPrints.data(), &ret.numRows, &ret.rowSizeInBytes, &ret.totalBytes);
	}

	uint64_t GraphicResource::GetResByteSize() const
	{
		assert(mResource);

		UINT64 ret(0);
		ID3D12Resource* res = Resource();
		ID3D12Device* dev = GetDevice();
		if (res && dev)
		{
			D3D12_RESOURCE_DESC desc = GetDesc();
			UINT subRes = desc.DepthOrArraySize * desc.MipLevels;
			dev->GetCopyableFootprints(&desc, 0, subRes, 0, nullptr, nullptr, nullptr, &ret);
		}

		return ret;
	}

	void GraphicResource::SetName(const wchar_t* name)
	{
		assert(mResource);
		mResource->SetName(name);
	}

	void GraphicResource::GetName(wchar_t* name, uint32_t bufferLength) const
	{
		assert(mResource);
		//mResource->GetPrivateData(WKPDID_D3DDebugObjectNameW, &bufferLength, (void*)name);
	}

	uint8_t* GraphicResource::Map(uint32_t subResources, const D3D12_RANGE* range)
	{
		assert(mResource);
		uint8_t* mapAddress(nullptr);
		ThrowIfFailed(mResource->Map(subResources, range, reinterpret_cast<void**>(&mapAddress)));
		return mapAddress;
	}

	void GraphicResource::Unmap(uint32_t subResources, const D3D12_RANGE* range)
	{
		assert(mResource);
		mResource->Unmap(subResources, range);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GraphicResource::GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
	{
		return mBindDescriptor[type][index].CpuHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GraphicResource::GetGpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
	{
		return mBindDescriptor[type][index].GpuHandle;
	}

	GraphicResource::operator bool()
	{
		return mResource;
	}

	void GraphicResource::InsertBindDescriptor(const DescriptorHandle& descriptorHandle)
	{
		mBindDescriptor[descriptorHandle.Type].push_back(descriptorHandle);

		if (is_gc_managed(descriptorHandle.DescriptorHeap)) 
		{
			mVecGcDescriptor.push_back(get_gc_ptr_from_raw(descriptorHandle.DescriptorHeap));
		}
	}

	void GraphicResource::ClearDescriptorHeap(DescriptorHeap* heap)
	{
		D3D12_DESCRIPTOR_HEAP_TYPE type = heap->GetType();

		auto it = mBindDescriptor[type].begin();
		while (it != mBindDescriptor[type].end())
		{
			DescriptorHandle& handle = *it;
			if (handle.DescriptorHeap == heap)
			{
				it = mBindDescriptor[type].erase(it);
			}
			else 
			{
				it++;
			}
		}

		
	}
};

