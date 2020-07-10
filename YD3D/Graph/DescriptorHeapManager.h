#pragma once
#include "YD3D_Header.h"
#include "Graph/DescriptorHeap.h"
#include "Resource/GraphicResource.h"

namespace YD3D
{
	class DescriptorHeapManager
	{
	public:
		DescriptorHeapManager();
		~DescriptorHeapManager();

		bool Create(ID3D12Device* device, uint32_t count);

		bool BindCbView(uint32_t index, const D3D12_CONSTANT_BUFFER_VIEW_DESC *view);
		bool BindCbView(uint32_t index, uint32_t count, const D3D12_CONSTANT_BUFFER_VIEW_DESC* vecView);

		bool BindSrView(uint32_t index, GraphicResource *res, const D3D12_SHADER_RESOURCE_VIEW_DESC *view);
		bool BindSrView(uint32_t index, uint32_t count, GraphicResource* vecRes, const D3D12_SHADER_RESOURCE_VIEW_DESC *vecView);

		bool BindUaView(uint32_t index, GraphicResource *res, GraphicResource *countRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC *view);
		bool BindUaView(uint32_t index, uint32_t count,  GraphicResource* arrRes, GraphicResource* arrCountRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* vecView);

		bool BindRtView(uint32_t index, GraphicResource *res, const D3D12_RENDER_TARGET_VIEW_DESC *view);
		bool BindRtView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_RENDER_TARGET_VIEW_DESC* view);

		bool BindDsView(uint32_t index, GraphicResource *res, const D3D12_DEPTH_STENCIL_VIEW_DESC *view);
		bool BindDsView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_DEPTH_STENCIL_VIEW_DESC* view);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index = 0);
		D3D12_CPU_DESCRIPTOR_HANDLE GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res);
		D3D12_GPU_DESCRIPTOR_HANDLE GetResourceGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res);
		GraphicResource* GetBindResource(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index);
		bool RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res);
		bool RemoveAllView(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res);
		bool IsOccupied(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t start, uint32_t count, bool* isOccupied);

		DescriptorHeap& GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

	private:
		ID3D12Device* mDevice;
		
		DescriptorHeap	mCbvSrvUavHeap;
		DescriptorHeap	mRtHeap;
		DescriptorHeap	mDsHeap;
		DescriptorHeap	mSamplerHeap;
	};
};
