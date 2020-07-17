#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicConstBuffer.hpp"
#include "Resource/GraphicTexture.h"
#include "Helper/WICImage.h"

namespace YD3D
{
	ALIGN_16 struct ModelInfo
	{
		DirectX::XMFLOAT4X4         ModelMatrix = Identity4x4();
		ALIGN_16 DirectX::XMFLOAT3  Position = {};
		ALIGN_16 DirectX::XMFLOAT3  Rotation = {};
		ALIGN_16 DirectX::XMFLOAT3  Scalar = { 1.0f, 1.0f, 1.0f };
	};
	
	struct ModelGraphicResource 
	{
		gc_ptr<GraphicConstBuffer<ModelInfo, 1>>	mGrpModelInfo;
		gc_ptr<GraphicTexture>						mTextures[8];
		gc_ptr<GraphicUploadBuffer>					mUploader;
	};

	class Model : public virtual enable_gc_ptr_form_raw
	{
		struct TextureLayout
		{
			TextureLayout() : 
				Offset(0),
				Length(0)
			{

			}

			uint64_t	Offset;
			uint64_t	Length;

			void Clear() 
			{
				Offset = 0;
				Length = 0;
			}
		};

	public:
		enum { need_clear_up_gc_ptr = 0 };

		Model();
		~Model();

		bool Create(ID3D12Device *device, const std::vector<Vertex> &mesh, const std::vector<uint32_t> &index);
		bool Create(ID3D12Device *device, std::vector<Vertex>&& mesh, std::vector<uint32_t>&& index);
		uint64_t VertexCount() const;
		uint64_t VertexSize() const;
		uint64_t IndexCount() const;
		uint64_t IndexSize() const;
		const Vertex* Vertices() const;
		const uint32_t* Indices() const;

		GraphicConstBuffer<ModelInfo, 1>* GraphicModelInfo();
		void UpdateModelInfo();

		ModelGraphicResource& GraphicResource();
		void UpdateTexture(YD3D::WICImage *images, uint32_t count);
		void UpdateGraphicResource(ID3D12GraphicsCommandList *commandList);
		
	private:
		ID3D12Device*								mDevice;
		std::vector<Vertex>							mMesh;
		std::vector<uint32_t>						mIndex;
		gc_ptr<GraphicConstBuffer<ModelInfo, 1>>	mGrpModelInfo;
		ModelInfo									mModelInfo;
		uint32_t									mTextureCount;
		TextureLayout								mTextureLayouts[8];
		ModelGraphicResource						mGpuResource;
	};
}
