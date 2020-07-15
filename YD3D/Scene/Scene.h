#pragma once
#include "YD3D_Header.h"
#include "Camera.h"
#include "Model.h"
#include "Resource/GraphicVertexBuffer.h"
#include "Resource/GraphicIndexBuffer.h"
#include "Resource/GraphicUploadBuffer.h"
#include "Resource/GraphicConstBuffer.hpp"

namespace YD3D
{
	struct DrawParam 
	{
		DrawParam() :
			Model(nullptr),
			IndexCountPerInstance(0),
			StartIndexLocation(0),
			BaseVertexLocation(0),
			StartInstanceLocation(0)
		{
		}

		Model*		Model;
		uint32_t	IndexCountPerInstance;
		uint32_t	StartIndexLocation;	
		int32_t		BaseVertexLocation;
		uint32_t	StartInstanceLocation;
	};

	typedef std::vector<gc_ptr<Model>, gc_allocator<gc_ptr<Model>>> VecModel;
	typedef std::unordered_map<Model*, DrawParam>					MapDrawParam;

	ALIGN_16 struct SceneInfo
	{
		DirectX::XMFLOAT4X4 View = Identity4x4();
		DirectX::XMFLOAT4X4 Project = Identity4x4();
		DirectX::XMFLOAT4X4 ViewProject = Identity4x4();
		ALIGN_16 DirectX::XMFLOAT3	CameraPos = {};
		ALIGN_16 DirectX::XMFLOAT3	CameraDir = {};
	};

	enum ESceneState { FREE = 0, SCENE_UPDATING = 0x01, RENDER_UPLOADING = 0x02, DIRTY = 0x04 };

	class Scene : public enable_gc_ptr_form_raw
	{
	public:
		enum { need_clear_up_gc_ptr = 0 };

		Scene();
		~Scene();

		bool Create(ID3D12Device *device);
		bool AddModel(const Model *model);
		void UpdateGraphicResource(bool wait = false);

		const D3D12_INDEX_BUFFER_VIEW& IndexView();
		const D3D12_VERTEX_BUFFER_VIEW& VertexView();
		const MapDrawParam& GetDrawParam();
		
		GraphicConstBuffer<SceneInfo, 1>* GraphicSceneInfo();
		Camera& GetCamera();
		void UpdateSceneInfo();
		ystate<ESceneState>			State;

	private:
		ID3D12Device*				mDevice;
		uint64_t					mVertexBufferLength;
		uint64_t					mIndexBufferLength;
		GraphicVertexBuffer			mVertexBuffer;
		GraphicIndexBuffer			mIndexBuffer;
		GraphicUploadBuffer			mUploadBuffer;
		VecModel					mModels;
		MapDrawParam				mDrawParam;

		Camera										mCamera;
		gc_ptr<GraphicConstBuffer<SceneInfo, 1>>	mGrpSceneInfo;
		SceneInfo									mSceneInfo;
		
		bool UploadTask(ID3D12GraphicsCommandList *commandList);
		uint64_t PostUploadTask();
	};
};

