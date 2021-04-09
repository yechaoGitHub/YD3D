#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicResource.h"

namespace YD3D
{
	class DescriptorHeap : public enable_gc_ptr_form_raw
	{
		using VecGcResource = std::vector<gc_ptr<GraphicResource>, gc_allocator<gc_ptr<GraphicResource>>>;
		friend class DescriptorHeapManager;
		friend class GraphicResource;

	public:
		enum { need_clear_up_gc_ptr = 1 };

		DescriptorHeap();
		~DescriptorHeap();

		bool Create(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t numDescriptors, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
		void Release();
		ID3D12DescriptorHeap* Descriptor() const;
		uint32_t GetHandleIncreasement();
		D3D12_DESCRIPTOR_HEAP_DESC GetDesc();
		D3D12_DESCRIPTOR_HEAP_TYPE GetType();
		ID3D12Device* GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t Index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t Index = 0);
		D3D12_CPU_DESCRIPTOR_HANDLE	GetResourceCpuHandle(GraphicResource* res);
		D3D12_GPU_DESCRIPTOR_HANDLE GetResourceGpuHandle(GraphicResource* res);

		bool RemoveView(GraphicResource *res);
		GraphicResource* GetSlot(uint32_t index);
		bool IsOccupied(uint32_t start, uint32_t count, bool* isOccupied);
		bool FindFreeSlot(uint32_t count, uint32_t* slotIndex);

		void clear_up_gc_ptr();

	private:
		ID3D12Device*											mDevice;
		ID3D12DescriptorHeap*									mDescriptorHeap;
		uint32_t												mDescriptorCount;
		uint32_t												mIncreasement;
		std::unordered_map<uint32_t, GraphicResource*>			mMapSlot;
		std::unordered_map<GraphicResource*, uint32_t>			mMapResource;
		VecGcResource											mGcResource;

		void SetSlot(uint32_t index, GraphicResource* res);
	};
};
