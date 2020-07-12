#include "DescriptorHeapManager.h"

namespace YD3D
{
	DescriptorHeapManager* DescriptorHeapManager::_GOBAL_DESCRIPTOR_HEAP_MANAGER_(nullptr);

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
		mCbvSrvUavHeap.assign(new DescriptorHeap);
		mRtHeap.assign(new DescriptorHeap);
		mDsHeap.assign(new DescriptorHeap);
		mSamplerHeap.assign(new DescriptorHeap);

		assert(mCbvSrvUavHeap->Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));
		assert(mRtHeap->Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
		assert(mDsHeap->Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, count, D3D12_DESCRIPTOR_HEAP_FLAG_NONE));
		assert(mSamplerHeap->Create(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, count, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

		return true;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindCbView(uint32_t index, const D3D12_CONSTANT_BUFFER_VIEW_DESC *view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(1, &index)) 
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, 1, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap->GetCpuHandle(index);
		D3D12_RESOURCE_DESC desc = {};
		mDevice->CreateConstantBufferView(view, handle);
		mCbvSrvUavHeap->SetSlot(index, nullptr);

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindCbView(uint32_t index, uint32_t count, const D3D12_CONSTANT_BUFFER_VIEW_DESC* vecView)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(count, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, count, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = BindCbView(index, &vecView[0]);
		for (UINT i = 1; i < count; i++)
		{
			BindCbView(index + i, &vecView[i]);
		}

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindSrView(uint32_t index, GraphicResource* res, const D3D12_SHADER_RESOURCE_VIEW_DESC* view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(1, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, 1, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap->GetCpuHandle(index);

		mDevice->CreateShaderResourceView(res->Resource(), view, handle);
		mCbvSrvUavHeap->SetSlot(index, res);
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindSrView(uint32_t index, uint32_t count, GraphicResource* vecRes, const D3D12_SHADER_RESOURCE_VIEW_DESC* vecView)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(count, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, count, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = BindSrView(index, &vecRes[0], &vecView[0]);
		for (uint32_t i = 1; i < count; i++)
		{
			BindSrView(index + i, &vecRes[i], &vecView[i]);
		}

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindUaView(uint32_t index, GraphicResource *res, GraphicResource *countRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(1, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, 1, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mCbvSrvUavHeap->GetCpuHandle(index);
		mDevice->CreateUnorderedAccessView(res->Resource(), countRes->Resource(), view, handle);
		mCbvSrvUavHeap->SetSlot(index, res);

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindUaView(uint32_t index, uint32_t count, GraphicResource* arrRes, GraphicResource* arrCountRes, const D3D12_UNORDERED_ACCESS_VIEW_DESC* vecView)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mCbvSrvUavHeap->FindFreeSlot(count, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mCbvSrvUavHeap->IsOccupied(index, count, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = BindUaView(index, &arrRes[0], &arrCountRes[0], vecView);
		for (uint32_t i = 1; i < count; i++)
		{
			BindUaView(index + i, &arrRes[i], &arrCountRes[i], vecView);
		}
		
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindRtView(uint32_t index, GraphicResource *res, const D3D12_RENDER_TARGET_VIEW_DESC *view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mRtHeap->FindFreeSlot(1, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mRtHeap->IsOccupied(index, 1, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mRtHeap->GetCpuHandle(index);
		mDevice->CreateRenderTargetView(res->Resource(), view, handle);
		mRtHeap->SetSlot(index, res);

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindRtView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_RENDER_TARGET_VIEW_DESC* view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mRtHeap->FindFreeSlot(count, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mRtHeap->IsOccupied(index, count, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = BindRtView(index, &arrRes[0], &view[0]);
		for (UINT i = 1; i < count; i++)
		{
			BindRtView(index + i, &arrRes[i], &view[i]);
		}

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindDsView(uint32_t index, GraphicResource *res, const D3D12_DEPTH_STENCIL_VIEW_DESC *view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mDsHeap->FindFreeSlot(1, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mDsHeap->IsOccupied(index, 1, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = mDsHeap->GetCpuHandle(index);
		mDevice->CreateDepthStencilView(res->Resource(), view, handle);
		mDsHeap->SetSlot(index, res);

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::BindDsView(uint32_t index, uint32_t count, GraphicResource* arrRes, const D3D12_DEPTH_STENCIL_VIEW_DESC* view)
	{
		if (index == ANY_DESCRIPTOR_HEAP_POS)
		{
			if (!mDsHeap->FindFreeSlot(count, &index))
			{
				return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
			}
		}

		bool occupied(false);
		if (mDsHeap->IsOccupied(index, count, &occupied) && occupied) return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

		D3D12_CPU_DESCRIPTOR_HANDLE handle = BindDsView(index, &arrRes[0], &view[0]);
		for (UINT i = 0; i < count; i++)
		{
			BindDsView(index + i, &arrRes[i], &view[i]);
		}

		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index)
	{
		return GetDescriptorHeap(type)->GetCpuHandle(Index);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t Index)
	{
		return GetDescriptorHeap(type)->GetGpuHandle(Index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res)
	{
		return GetDescriptorHeap(type)->GetResourceCpuHandle(res);
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetResourceGpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res)
	{
		return GetDescriptorHeap(type)->GetResourceGpuHandle(res);
	}

	GraphicResource* DescriptorHeapManager::GetBindResource(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
	{
		return GetDescriptorHeap(type)->GetSlot(index);
	}

	bool DescriptorHeapManager::RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE type, GraphicResource *res)
	{
		return GetDescriptorHeap(type)->RemoveView(res);
	}

	bool DescriptorHeapManager::RemoveAllView(GraphicResource *res)
	{
		mCbvSrvUavHeap->RemoveView(res);
		mRtHeap->RemoveView(res);
		mDsHeap->RemoveView(res);
		mSamplerHeap->RemoveView(res);

		return true;
	}

	bool DescriptorHeapManager::IsOccupied(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t start, uint32_t count, bool* isOccupied)
	{
		return GetDescriptorHeap(type)->IsOccupied(start, count, isOccupied);
	}

	bool DescriptorHeapManager::FindFreePos(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t count, uint32_t* pos)
	{
		return GetDescriptorHeap(type)->FindFreeSlot(count, pos);
	}

	DescriptorHeap* DescriptorHeapManager::GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
	{
		switch (type)
		{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
				return mCbvSrvUavHeap.get_raw_ptr();

			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
				return mSamplerHeap.get_raw_ptr();

			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
				return mRtHeap.get_raw_ptr();

			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
				return mDsHeap.get_raw_ptr();

			default:
				assert(0);
			break;
		}

		assert(0);
	}

	void DescriptorHeapManager::SetGobalDescriptorHeapManager(DescriptorHeapManager &manager)
	{
		_GOBAL_DESCRIPTOR_HEAP_MANAGER_ = &manager;
	}

	DescriptorHeapManager* DescriptorHeapManager::GobalDescriptorHeapManager()
	{
		return _GOBAL_DESCRIPTOR_HEAP_MANAGER_;
	}
};
