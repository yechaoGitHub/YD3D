#include "GraphicResource.h"

namespace YD3D
{
	GraphicResource::GraphicResource() :
		mResource(nullptr)
	{
	}

	GraphicResource::~GraphicResource()
	{
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

	void GraphicResource::GetFootPrint(CopyableFootPrint& footPrint) const
	{
		assert(mResource);

		CopyableFootPrint& ret = footPrint;
		D3D12_RESOURCE_DESC desc = GetDesc();
		ID3D12Device* dev = GetDevice();

		UINT subresourceNum = desc.DepthOrArraySize * desc.MipLevels;
		ret.footPrints.assign(subresourceNum, D3D12_PLACED_SUBRESOURCE_FOOTPRINT());

		dev->GetCopyableFootprints(&desc, 0, subresourceNum, 0, ret.footPrints.data(), &ret.numRows, &ret.rowSizeInBytes, &ret.totalBytes);
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

	void* GraphicResource::Map(uint32_t subResources, const D3D12_RANGE* range)
	{
		assert(mResource);
		void* mapAddress(nullptr);
		ThrowIfFailed(mResource->Map(subResources, range, &mapAddress));
		return mapAddress;
	}

	void GraphicResource::Unmap(uint32_t subResources, const D3D12_RANGE* range)
	{
		assert(mResource);
		mResource->Unmap(subResources, range);
	}

};
