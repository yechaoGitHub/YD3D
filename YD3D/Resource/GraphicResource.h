#pragma once
#include "YD3D_Header.h"

namespace YD3D 
{
	typedef std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> vecFootPrint;

	class DescriptorHeap;

	struct CopyableFootPrint
	{
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
			mType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
			mSlotIndex(0),
			mCpuHandle({0}),
			mGpuHandle({0}),
			mView({})
		{
		}

		D3D12_DESCRIPTOR_HEAP_TYPE		mType;
		gc_ptr<DescriptorHeap>			mDescriptorHeap;
		uint32_t						mSlotIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE		mCpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE		mGpuHandle;
		ResourceView					mView;
	};

	class GraphicResource : public virtual enable_gc_ptr_form_raw
	{
		typedef std::array<std::vector<DescriptorHandle>, 5> DescriptorHandleContainer;
		friend class DescriptorHeapManager;

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
		void GetFootPrint(CopyableFootPrint& footPrint) const;
		uint64_t GetResByteSize() const;

		void SetName(const wchar_t* name);
		void GetName(wchar_t* name, uint32_t bufferLength) const;

		void* Map(uint32_t subResources, const D3D12_RANGE* range);
		void Unmap(uint32_t subResources, const D3D12_RANGE* range);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index = 0);

	private:
		ID3D12Resource*				mResource;
		DescriptorHandleContainer	mBindDescriptor;

		void InsertBindDescriptor(const DescriptorHandle& descriptorHandle);

	};
};