#pragma once
#include "YD3D_Header.h"

namespace YD3D 
{
	typedef std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> vecFootPrint;

	struct CopyableFootPrint
	{
		uint32_t			numRows;
		uint64_t			rowSizeInBytes;
		uint64_t			totalBytes;
		vecFootPrint		footPrints;
	};
	
	class GraphicResource : public virtual enable_gc_ptr_form_raw
	{
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

	private:
		ID3D12Resource* mResource;
	};
};