#pragma once
#include "YD3D_Header.h"

namespace YD3D 
{
	typedef std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> vecFootPrint;

	class DescriptorHeap;

	struct CopyableFootPrint
	{
		CopyableFootPrint() :
			numRows(0),
			rowSizeInBytes(0),
			totalBytes(0)
		{

		}

		uint32_t			numRows;
		uint64_t			rowSizeInBytes;
		uint64_t			totalBytes;
		vecFootPrint		footPrints;
	};
	
	union ResourceView
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC		ConstBufferView;
		D3D12_SHADER_RESOURCE_VIEW_DESC		ShaderResourceView;
		D3D12_UNORDERED_ACCESS_VIEW_DESC	UnorderedAccessView;
		D3D12_RENDER_TARGET_VIEW_DESC		RenderTargetView;
		D3D12_DEPTH_STENCIL_VIEW_DESC		DepthStencilView;
	};

	struct DescriptorHandle
	{
		DescriptorHandle() :
			Type(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			SlotIndex(0),
			CpuHandle({0}),
			GpuHandle({0}),
			View({})
		{
		}

		D3D12_DESCRIPTOR_HEAP_TYPE		Type;
		DescriptorHeap*					DescriptorHeap;
		uint32_t						SlotIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE		CpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE		GpuHandle;
		ResourceView					View;
	};

	class GraphicResource : public virtual enable_gc_ptr_form_raw
	{
		using DescriptorHandleContainer = std::array<std::vector<DescriptorHandle>, 5>;
		using VecGcDescriptor = std::vector<gc_ptr<DescriptorHeap>, gc_allocator<gc_ptr<DescriptorHeap>>>;
		friend class DescriptorHeapManager;
		friend class DescriptorHeap;

	public:
		enum { need_clear_up_gc_ptr = 0 };

		GraphicResource();
		virtual ~GraphicResource();

		bool Create(ID3D12Device* device, D3D12_HEAP_PROPERTIES* heapProperty, D3D12_HEAP_FLAGS flag, D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES state, D3D12_CLEAR_VALUE* clearValue);
		virtual void Release();
		ID3D12Resource* Resource() const;
		ID3D12Device* GetDevice() const;
		ID3D12Resource** GetResourceAddressOf();
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress() const;
		D3D12_RESOURCE_DESC GetDesc() const;
		D3D12_HEAP_PROPERTIES GetHeapProperties() const;
		void GetFootPrint(CopyableFootPrint& footPrint, uint64_t offset = 0) const;
		uint64_t GetResByteSize() const;

		void SetName(const wchar_t* name);
		void GetName(wchar_t* name, uint32_t bufferLength) const;

		uint8_t* Map(uint32_t subResources, const D3D12_RANGE* range);
		void Unmap(uint32_t subResources, const D3D12_RANGE* range);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index = 0);

		operator bool();

	private:
		ID3D12Resource*				mResource;
		DescriptorHandleContainer	mBindDescriptor;
		VecGcDescriptor				mVecGcDescriptor;

		void InsertBindDescriptor(const DescriptorHandle& descriptorHandle);
		void ClearDescriptorHeap(DescriptorHeap* heap);

	};
};