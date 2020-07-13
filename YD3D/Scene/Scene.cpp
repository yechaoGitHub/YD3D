#pragma once
#include "Scene.h"
#include "Graph/GraphicTask.h"

namespace YD3D 
{
	Scene::Scene():
		mDevice(nullptr),
		mState(ESceneState::CLEAR),
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

		mCamera.SetLens(0.25f * Pi, 4.0/3.0, 0.1, 1000.0f);
		mGrpSceneInfo.assign(new GraphicConstBuffer<SceneInfo, 1>);
		assert(mGrpSceneInfo->Create(mDevice));
		UpdateGraphicResource(true);
		
		return true;
	}

	bool Scene::AddModel(const Model *model)
	{
		mModels.push_back(get_gc_ptr_from_raw(model));

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

			DrawParam drawParam;
			drawParam.Model = model.get_raw_ptr();
			drawParam.IndexCountPerInstance = model->IndexCount();
			drawParam.BaseVertexLocation = vertexOffset;
			drawParam.StartIndexLocation = indexOffset;
			mDrawParam.insert(std::make_pair(drawParam.Model, drawParam));
		}
		
		uint64_t fenceValue = PostUploadTask();
		if (wait) 
		{
			GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue, true);
		}
	}

	const D3D12_INDEX_BUFFER_VIEW& Scene::IndexView()
	{
		return mIndexBuffer.IndexView();
	}

	const D3D12_VERTEX_BUFFER_VIEW& Scene::VertexView()
	{
		return mVertexBuffer.VertexView();
	}

	const MapDrawParam& Scene::GetDrawParam()
	{
		return mDrawParam;
	}

	GraphicConstBuffer<SceneInfo, 1>* Scene::GraphicSceneInfo()
	{
		return mGrpSceneInfo.get_raw_ptr();
	}

	void Scene::CameraPos(const DirectX::XMFLOAT3& pos)
	{
		mCamera.SetPosition(pos);
	}

	void Scene::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
	{
		mCamera.LookAt(pos, target, up);
	}

	void Scene::UpdateSceneInfo()
	{
		mCamera.UpdateViewMatrix();
		mSceneInfo.CameraPos = mCamera.GetPosition3f();
		mSceneInfo.CameraDir = mCamera.GetLook3f();
		mSceneInfo.View = mCamera.GetView4x4f();
		mSceneInfo.Project = mCamera.GetProj4x4f();
		auto matViewProj = mCamera.GetView() * mCamera.GetProj();
		XMStoreFloat4x4(&mSceneInfo.ViewProject, matViewProj);

		mGrpSceneInfo->Update(0, mSceneInfo);
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
