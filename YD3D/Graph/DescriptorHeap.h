#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicResource.h"

namespace YD3D
{
	class DescriptorHeap 
	{
		friend class DescriptorHeapManager;
	public:
		DescriptorHeap();
		~DescriptorHeap();

		bool Create(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		ID3D12DescriptorHeap* Descriptor() const;
		uint32_t GetHandleIncreasement();
		D3D12_DESCRIPTOR_HEAP_DESC GetDesc();
		D3D12_DESCRIPTOR_HEAP_TYPE GetType();
		ID3D12Device* GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t Index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t Index = 0);
		D3D12_CPU_DESCRIPTOR_HANDLE	GetResourceCpuHandle(const GraphicResource& res);
		D3D12_GPU_DESCRIPTOR_HANDLE GetResourceGpuHandle(const GraphicResource& res);

		bool RemoveView(const GraphicResource& res);
		GraphicResource* GetSlot(uint32_t index);
		bool IsOccupied(uint32_t start, uint32_t count, bool* isOccupied);
		bool FindFreeSlot(uint32_t count, uint32_t* slotIndex);

	private:
		ID3D12Device*											mDevice;
		ID3D12DescriptorHeap*									mDescriptorHeap;
		uint32_t												mDescriptorCount;
		uint32_t												mIncreasement;
		std::unordered_map<uint32_t, GraphicResource*>			mMapResource;

		void SetSlot(uint32_t index, GraphicResource* res);
	};
};
