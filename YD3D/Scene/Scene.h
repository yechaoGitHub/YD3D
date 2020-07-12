#pragma once
#include "YD3D_Header.h"
#include "Model.h"
#include "Resource/GraphicVertexBuffer.h"
#include "Resource/GraphicIndexBuffer.h"
#include "Resource/GraphicUploadBuffer.h"

namespace YD3D
{
	enum ESceneState { CLEAR, UPLOADING, DIRTY };

	class Scene : public enable_gc_ptr_form_raw
	{
	public:
		enum { need_clear_up_gc_ptr = 0 };

		Scene();
		~Scene();

		bool Create(ID3D12Device *device);
		bool AddModel(const gc_ptr<Model>& model);
		void UpdateGraphicResource(bool wait = false);

	private:
		ID3D12Device*				mDevice;
		ystate<ESceneState>			mState;
		uint64_t					mVertexBufferLength;
		uint64_t					mIndexBufferLength;
		GraphicVertexBuffer			mVertexBuffer;
		GraphicIndexBuffer			mIndexBuffer;
		GraphicUploadBuffer			mUploadBuffer;
		std::vector<gc_ptr<Model>, gc_allocator<gc_ptr<Model>>> mModels;

		bool UploadTask(ID3D12GraphicsCommandList *commandList);
		uint64_t PostUploadTask();
	};
};

