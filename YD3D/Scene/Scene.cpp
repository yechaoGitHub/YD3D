#pragma once
#include "Scene.h"
#include "Graph/GraphicTask.h"

namespace YD3D 
{
	Scene::Scene():
		mDevice(nullptr),
		mState(CLEAR),
		mVertexBufferLength(0),
		mIndexBufferLength(0),
		mModels(get_gc_allocator<gc_ptr<Model>>())
	{
	}

	Scene::~Scene()
	{
	}

	bool Scene::Create(ID3D12Device* device)
	{
		mDevice = device;
		assert(mVertexBuffer.Create(mDevice, sizeof(Vertex), 1024));
		assert(mIndexBuffer.Create(mDevice, 1024, DXGI_FORMAT_R32_UINT));
		assert(mUploadBuffer.Create(mDevice, 1024));

		return true;
	}

	bool Scene::AddModel(const gc_ptr<Model>& model)
	{
		mModels.push_back(model);

		mVertexBufferLength += model->VertexSize();
		mIndexBufferLength += model->IndexSize();

		return true;
	}

	void Scene::UpdateGraphicResource(bool wait)
	{
		if (mVertexBufferLength > mVertexBuffer.BufferLength())
		{
			mVertexBuffer.Release();
			mVertexBuffer.Create(mDevice, sizeof(Vertex), mVertexBufferLength / sizeof(Vertex));
		}

		if (mIndexBufferLength > mIndexBuffer.BufferLength())
		{
			mIndexBuffer.Release();
			mIndexBuffer.Create(mDevice, mIndexBufferLength, DXGI_FORMAT_R32_UINT);
		}

		uint64_t uploadSize = mVertexBufferLength + mIndexBufferLength;
		if (uploadSize > mUploadBuffer.GetResByteSize())
		{
			mUploadBuffer.Release();
			mUploadBuffer.Create(mDevice, uploadSize);
		}

		uint64_t vertexOffset(0);
		uint64_t indexOffset(0);
		for (auto &model : mModels) 
		{
			uint64_t curVertexSize = model->VertexSize();
			mUploadBuffer.CopyData(vertexOffset, reinterpret_cast<const BYTE*>(model->Vertices()), curVertexSize);
			vertexOffset += curVertexSize;

			uint64_t curIndexSize = model->IndexSize();
			mUploadBuffer.CopyData(indexOffset + mVertexBufferLength, reinterpret_cast<const BYTE*>(model->Indices()), curIndexSize);
			indexOffset += curIndexSize;
		}
		
		uint64_t fenceValue = PostUploadTask();
		if (wait) 
		{
			GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue, true);
		}
	}

	bool Scene::UploadTask(ID3D12GraphicsCommandList* commandList)
	{
		CD3DX12_RESOURCE_BARRIER barrier[4];

		barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(mIndexBuffer.Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		barrier[2] = CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		barrier[3] = CD3DX12_RESOURCE_BARRIER::Transition(mIndexBuffer.Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);

		commandList->ResourceBarrier(2, &barrier[0]);

		commandList->CopyBufferRegion(mVertexBuffer.Resource(), 0, mUploadBuffer.Resource(), 0, mVertexBufferLength);
		commandList->CopyBufferRegion(mIndexBuffer.Resource(), 0, mUploadBuffer.Resource(), mVertexBufferLength, mIndexBufferLength);

		commandList->ResourceBarrier(2, &barrier[2]);

		return true;
	}

	uint64_t Scene::PostUploadTask()
	{		
		uint64_t fenceValue(0);
		GraphicTaskFunction task = std::bind(&Scene::UploadTask, this, std::placeholders::_1);
		GraphicTask::PostGraphicTask(ECommandQueueType::ECOPY, std::move(task), &fenceValue);
		return fenceValue;
	}
}
