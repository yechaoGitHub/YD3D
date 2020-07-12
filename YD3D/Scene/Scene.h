#pragma once
#include "YD3D_Header.h"
#include "Model.h"
#include "Resource/GraphicVertexBuffer.h"
#include "Resource/GraphicIndexBuffer.h"
#include "Resource/GraphicUploadBuffer.h"

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
		
	private:
		ID3D12Device*				mDevice;
		ystate<ESceneState>			mState;
		uint64_t					mVertexBufferLength;
		uint64_t					mIndexBufferLength;
		GraphicVertexBuffer			mVertexBuffer;
		GraphicIndexBuffer			mIndexBuffer;
		GraphicUploadBuffer			mUploadBuffer;
		VecModel					mModels;
		MapDrawParam				mDrawParam;

		bool UploadTask(ID3D12GraphicsCommandList *commandList);
		uint64_t PostUploadTask();
	};
};

