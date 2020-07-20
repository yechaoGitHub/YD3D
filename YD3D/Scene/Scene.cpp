#pragma once
#include "Scene.h"
#include "Graph/GraphicTask.h"

namespace YD3D 
{
	using namespace DirectX;

	Scene::Scene():
		mDevice(nullptr),
		mVertexBufferLength(0),
		mIndexBufferLength(0),
		mModels(get_gc_allocator<gc_ptr<Model>>()),
		mState(ESceneState::FREE)
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

		SetLens(0.25f * Pi, 4.0/3.0, 0.1, 1000.0f);
		LookAt(XMFLOAT3{ 0, 0, -3 }, XMFLOAT3{ 0,0,0 }, XMFLOAT3{0, 1, 0});
		mGpuSceneInfo.assign(new GraphicConstBuffer<SceneInfo>);
		assert(mGpuSceneInfo->Create(mDevice, 1));
		
		return true;
	}

	bool Scene::AddModel(const Model *model)
	{
		mModels.push_back(get_gc_ptr_from_raw(model));

		mVertexBufferLength += model->VertexSize();
		mIndexBufferLength += model->IndexSize();

		mState.add_state(DRAW_PARAM_DIRTY);

		return true;
	}

	void Scene::UpdateGraphicResource(bool wait)
	{
		if (mState.has_state_strong(CAMERA_DIRTY))
		{
			mCamera.UpdateViewMatrix();

			SceneInfo* gpuSceneInfo = reinterpret_cast<SceneInfo*>(mGpuSceneInfo->GetMappedPointer());

			gpuSceneInfo->CameraPos = mCamera.GetPosition3f();
			gpuSceneInfo->CameraDir = mCamera.GetLook3f();
			gpuSceneInfo->View = mCamera.GetView4x4f();
			gpuSceneInfo->Project = mCamera.GetProj4x4f();
			auto matViewProj = XMMatrixMultiply(mCamera.GetView(), mCamera.GetProj());
			XMStoreFloat4x4(&gpuSceneInfo->ViewProject, matViewProj);

			mState.remove_state(CAMERA_DIRTY);
		}

		if (mState.has_state_strong(LIGHT_DIRTY))
		{
			SceneInfo* gpuSceneInfo = reinterpret_cast<SceneInfo*>(mGpuSceneInfo->GetMappedPointer());
			gpuSceneInfo->PointLightCount = mPointLights.size();
			gpuSceneInfo->SpotLightCount = mSpotLights.size();

			uint32_t index(0);
			for (auto &light : mPointLights) 
			{
				mGpuLightInfo->Update(index, light);
				index++;
			}

			for (auto &light : mSpotLights) 
			{
				mGpuLightInfo->Update(index, light);
				index++;
			}

			mState.remove_state(LIGHT_DIRTY);
		}

		if (mState.has_state_strong(VERTEX_INDEX_DIRTY))
		{
			uint64_t fenceValue(0);
			GraphicTask::PostGraphicTask(
				ECommandQueueType::ECOPY, 
				[this](ID3D12GraphicsCommandList* commandList)
				{
					return GraphicTaskUploadVertexIndexData(commandList);
				}, 
				&fenceValue,
				[this] (D3D12_COMMAND_LIST_TYPE, uint64_t)
				{
					mState.remove_state(VERTEX_INDEX_DIRTY);
				});

			if (wait) 
			{
				GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue);
			}
		}
	}

	void Scene::UpdateDrawParam()
	{
		if (mState.has_state_strong(DRAW_PARAM_DIRTY))
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
			for (auto& model : mModels)
			{
				uint64_t curVertexSize = model->VertexSize();
				mUploadBuffer.CopyData(vertexOffset, reinterpret_cast<const BYTE*>(model->Vertices()), curVertexSize);

				uint64_t curIndexSize = model->IndexSize();
				mUploadBuffer.CopyData(indexOffset + mVertexBufferLength, reinterpret_cast<const BYTE*>(model->Indices()), curIndexSize);

				DrawParam drawParam;
				drawParam.Model = model.get_raw_ptr();
				drawParam.IndexCountPerInstance = model->IndexCount();
				drawParam.BaseVertexLocation = vertexOffset;
				drawParam.StartIndexLocation = indexOffset;
				mDrawParam.insert(std::make_pair(drawParam.Model, drawParam));
				vertexOffset += curVertexSize;
				indexOffset += curIndexSize;

				model->UpdateModelInfo();
			}

			mState.transfer_state(DRAW_PARAM_DIRTY, VERTEX_INDEX_DIRTY);
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

	void Scene::Walk(float value)
	{
		mCamera.Walk(value);
		mState.add_state(CAMERA_DIRTY);
	}

	void Scene::Strafe(float value)
	{
		mCamera.Strafe(value);
		mState.add_state(CAMERA_DIRTY);
	}

	void Scene::Pitch(float angle)
	{
		mCamera.Pitch(angle);
		mState.add_state(CAMERA_DIRTY);
	}

	void Scene::RotateY(float angle)
	{
		mCamera.RotateY(angle);
		mState.add_state(CAMERA_DIRTY);
	}

	void Scene::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
	{
		mCamera.LookAt(pos, target, up);
		mState.add_state(CAMERA_DIRTY);
	}

	void Scene::SetLens(float fovY, float aspect, float zn, float zf)
	{
		mCamera.SetLens(fovY, aspect, zn, zf);
		mState.add_state(CAMERA_DIRTY);
	}

	GraphicConstBuffer<SceneInfo>* Scene::GraphicSceneInfo()
	{
		return mGpuSceneInfo.get_raw_ptr();
	}

	GraphicConstBuffer<LightDataStruct>* Scene::GraphicLightInfo()
	{
		return mGpuLightInfo.get_raw_ptr();
	}

	void Scene::AddPointLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& position)
	{
		mSceneInfo.PointLightCount++;

		LightDataStruct light;
		light.Strength = strength;
		light.Position = position;

		mPointLights.push_back(light);

		mState.add_state(LIGHT_PARAM_DIRTY);
	}

	void Scene::AddSpotLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction)
	{
		mSceneInfo.SpotLightCount++;

		LightDataStruct light;
		light.Strength = strength;
		light.Position = position;
		light.Diretion = direction;

		mSpotLights.push_back(light);

		mState.add_state(LIGHT_PARAM_DIRTY);
	}

	void Scene::UpdateLightParam()
	{
		if (mState.has_state_strong(LIGHT_PARAM_DIRTY))
		{
			uint32_t lightCount = mPointLights.size() + mSpotLights.size();
			uint32_t gpuLightCount = mGpuLightInfo->ElemCount();

			if (gpuLightCount < lightCount) 
			{
				mGpuLightInfo->Release();
				mGpuLightInfo->Create(mDevice, gpuLightCount);
			}

			mState.transfer_state(LIGHT_PARAM_DIRTY, LIGHT_DIRTY);
		}
	}

	ystate<ESceneState>& Scene::State()
	{
		return mState;
	}

	bool Scene::StateBarrier(StateCheckFunction stateCheckFunc, StateSetFunction stateSetFunc)
	{
		if (stateCheckFunc(mState)) 
		{
			stateSetFunc(mState);
			return true;
		}
		else 
		{
			return false;
		}
	}

	bool Scene::GraphicTaskUploadVertexIndexData(ID3D12GraphicsCommandList* commandList)
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

}
