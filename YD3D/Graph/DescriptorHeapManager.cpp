#include "DescriptorHeapManager.h"

namespace YD3D
{
	DescriptorHeapManager::DescriptorHeapManager() :
		mDevice(nullptr)
	{
	}

	DescriptorHeapManager::~DescriptorHeapManager()
	{
	}

	bool DescriptorHeapManager::Create(ID3D12Device* device, uint32_t count)
	{
		mDevice = device;

		assert(mCbvSrvUavHeap.Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));
		assert(mRtHeap.Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
		assert(mDsHeap.Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
		assert(mSamplerHeap.Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

		return true;
	}

	bool DescriptorHeapManager::BindCbView(uint32_t index, const D3D12_CONSTANT_BUFFER_VIEW_DESC *view)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, 1, &occupied) && occupied) return false;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap.GetCpuHandle(index);
		D3D12_RESOURCE_DESC desc = {};

		mDevice->CreateConstantBufferView(view, handle);
		mCbvSrvUavHeap.SetSlot(index, nullptr);
	}

	bool DescriptorHeapManager::BindCbView(uint32_t index, uint32_t count, const D3D12_CONSTANT_BUFFER_VIEW_DESC* vecView)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, count, &occupied) && occupied) return false;

		for (UINT i = 0; i < count; i++)
		{
			BindCbView(index + i, &vecView[i]);
		}

		return true;
	}

	bool DescriptorHeapManager::BindSrView(uint32_t index, GraphicResource* res, const D3D12_SHADER_RESOURCE_VIEW_DESC* view)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, 1, &occupied) && occupied) return false;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap.GetCpuHandle(index);

		mDevice->CreateShaderResourceView(res->Resource(), view, handle);
		mCbvSrvUavHeap.SetSlot(index, res);
		return true;
	}

	bool DescriptorHeapManager::BindSrView(uint32_t index, uint32_t count, GraphicResource* vecRes, const D3D12_SHADER_RESOURCE_VIEW_DESC* vecView)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, count, &occupied) && occupied) return false;

		for (uint32_t i = 0; i < count; i++)
		{
			BindSrView(index + i, &vecRes[i], &vecView[i]);
		}

		return true;
	}

	bool DescriptorHeapManager::BindUaView(uint32_t index, GraphicResource *res, GraphicResource *countRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* view)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, 1, &occupied) && occupied) return false;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap.GetCpuHandle(index);

		mDevice->CreateUnorderedAccessView(res->Resource(), countRes->Resource(), view, handle);
		mCbvSrvUavHeap.SetSlot(index, res);

		return true;
	}

	bool DescriptorHeapManager::BindUaView(uint32_t index, uint32_t count, GraphicResource* arrRes, GraphicResource* arrCountRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* vecView)
	{
		bool occupied(false);
		if (mCbvSrvUavHeap.IsOccupied(index, count, &occupied) && occupied) return false;

		for (uint32_t i = 0; i < count; i++)
		{
			BindUaView(index + i, &arrRes[i], &arrCountRes[i], vecView);
		}
		
		return true;
	}

	bool DescriptorHeapManager::BindRtView(uint32_t index, GraphicResource *res, const D3D12_RENDER_TARGET_VIEW_DESC *view)
	{
		bool occupied(false);
		if (mRtHeap.IsOccupied(index, 1, &occupied) && occupied) return false;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mRtHeap.GetCpuHandle(index);

		mDevice->CreateRenderTargetView(res->Resource(), view, handle);
		mRtHeap.SetSlot(index, res);

		return true;
	}

	bool DescriptorHeapManager::BindRtView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_RENDER_TARGET_VIEW_DESC* view)
	{
		bool occupied(false);
		if (mRtHeap.IsOccupied(index, count, &occupied) && occupied) return false;

		for (UINT i = 0; i < count; i++)
		{
			BindRtView(index + i, &arrRes[i], &view[i]);
		}

		return true;
	}

	bool DescriptorHeapManager::BindDsView(uint32_t index, GraphicResource *res, const D3D12_DEPTH_STENCIL_VIEW_DESC *view)
	{
		bool occupied(false);
		if (mDsHeap.IsOccupied(index, 1, &occupied) && occupied) return false;

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mDsHeap.GetCpuHandle(index);

		mDevice->CreateDepthStencilView(res->Resource(), view, handle);
		mRtHeap.SetSlot(index, res);

		return true;
	}

	bool DescriptorHeapManager::BindDsView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_DEPTH_STENCIL_VIEW_DESC* view)
	{
		bool occupied(false);
		if (mRtHeap.IsOccupied(index, count, &occupied) && occupied) return false;

		for (UINT i = 0; i < count; i++)
		{
			BindDsView(index + i, &arrRes[i], &view[i]);
		}

		return true;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index)
	{
		return GetDescriptorHeap(type).GetCpuHandle(Index);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index)
	{
		return GetDescriptorHeap(type).GetGpuHandle(Index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res)
	{
		return GetDescriptorHeap(type).GetResourceCpuHandle(res);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetResourceGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res)
	{
		return GetDescriptorHeap(type).GetResourceGpuHandle(res);
	}

	GraphicResource* DescriptorHeapManager::GetBindResource(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
	{
		return GetDescriptorHeap(type).GetSlot(index);
	}

	bool DescriptorHeapManager::RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res)
	{
		return GetDescriptorHeap(type).RemoveView(res);
	}

	bool DescriptorHeapManager::RemoveAllView(D3D12_DESCRIPTOR_HEAP_TYPE type, const GraphicResource& res)
	{
		mCbvSrvUavHeap.RemoveView(res);
		mRtHeap.RemoveView(res);
		mDsHeap.RemoveView(res);
		mSamplerHeap.RemoveView(res);

		return true;
	}

	bool DescriptorHeapManager::IsOccupied(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t start, uint32_t count, bool* isOccupied)
	{
		return GetDescriptorHeap(type).IsOccupied(start, count, isOccupied);
	}

	DescriptorHeap& DescriptorHeapManager::GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
				return mCbvSrvUavHeap;

			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				return mSamplerHeap;

			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				return mRtHeap;

			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				return mDsHeap;

			default:
				assert(0);
			break;
		}

		assert(0);
	}
};
