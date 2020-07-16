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

		D3D12_CPU_DESCRIPTOR_HANDLE BindCbView(uint32_t index, GraphicResource *res, const D3D12_CONSTANT_BUFFER_VIEW_DESC *view);
		D3D12_CPU_DESCRIPTOR_HANDLE BindCbView(uint32_t index, uint32_t count, GraphicResource** res, const D3D12_CONSTANT_BUFFER_VIEW_DESC* vecView);

		D3D12_CPU_DESCRIPTOR_HANDLE BindSrView(uint32_t index, GraphicResource *res, const D3D12_SHADER_RESOURCE_VIEW_DESC *view);
		D3D12_CPU_DESCRIPTOR_HANDLE BindSrView(uint32_t index, uint32_t count, GraphicResource* vecRes, const D3D12_SHADER_RESOURCE_VIEW_DESC *vecView);

		D3D12_CPU_DESCRIPTOR_HANDLE BindUaView(uint32_t index, GraphicResource *res, GraphicResource *countRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC *view);
		D3D12_CPU_DESCRIPTOR_HANDLE BindUaView(uint32_t index, uint32_t count,  GraphicResource** arrRes, GraphicResource** arrCountRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* vecView);

		D3D12_CPU_DESCRIPTOR_HANDLE BindRtView(uint32_t index, GraphicResource *res, const D3D12_RENDER_TARGET_VIEW_DESC *view);
		D3D12_CPU_DESCRIPTOR_HANDLE BindRtView(uint32_t index, uint32_t count, GraphicResource** arrRes, const D3D12_RENDER_TARGET_VIEW_DESC* view);

		D3D12_CPU_DESCRIPTOR_HANDLE BindDsView(uint32_t index, GraphicResource *res, const D3D12_DEPTH_STENCIL_VIEW_DESC *view);
		D3D12_CPU_DESCRIPTOR_HANDLE BindDsView(uint32_t index, uint32_t count, GraphicResource** arrRes, const D3D12_DEPTH_STENCIL_VIEW_DESC* view);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index = 0);
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index = 0);
		D3D12_CPU_DESCRIPTOR_HANDLE GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res);
		D3D12_GPU_DESCRIPTOR_HANDLE GetResourceGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res);
		GraphicResource* GetBindResource(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index);
		bool RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res);
		bool RemoveAllView(GraphicResource *res);
		bool IsOccupied(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t start, uint32_t count, bool* isOccupied);
		bool FindFreePos(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t count, uint32_t* pos);

		DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

		static void SetGobalDescriptorHeapManager(DescriptorHeapManager &manager);
		static DescriptorHeapManager* GobalDescriptorHeapManager();

	private:
		ID3D12Device* mDevice;
		
		gc_ptr<DescriptorHeap>	mCbvSrvUavHeap;
		gc_ptr<DescriptorHeap>	mRtHeap;
		gc_ptr<DescriptorHeap>	mDsHeap;
		gc_ptr<DescriptorHeap>	mSamplerHeap;

		static DescriptorHeapManager* _GOBAL_DESCRIPTOR_HEAP_MANAGER_;
	};
};
