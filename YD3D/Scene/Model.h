#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicConstBuffer.hpp"

namespace YD3D
{
	ALIGN_16 struct ModelInfo
	{
		DirectX::XMFLOAT4X4         ModelMatrix = Identity4x4();
		ALIGN_16 DirectX::XMFLOAT3  Position = {};
		ALIGN_16 DirectX::XMFLOAT3  Rotation = {};
		ALIGN_16 DirectX::XMFLOAT3  Scalar = { 1.0f, 1.0f, 1.0f };
	};

	class Model : public virtual enable_gc_ptr_form_raw
	{
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
		const Vertex*	Vertices() const;
		const uint32_t* Indices() const;

		GraphicConstBuffer<ModelInfo, 1>* GraphicModelInfo();
		void UpdateModelInfo();
		
	private:
		ID3D12Device*								mDevice;
		std::vector<Vertex>							mMesh;
		std::vector<uint32_t>						mIndex;
		gc_ptr<GraphicConstBuffer<ModelInfo, 1>>	mGrpModelInfo;
		ModelInfo									mModelInfo;
	};
}
