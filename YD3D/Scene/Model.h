#pragma once
#include "YD3D_Header.h"

namespace YD3D
{
	class Model : public enable_gc_ptr_form_raw
	{
	public:
		Model();
		~Model();

		bool Create(const std::vector<Vertex> &mesh, const std::vector<uint32_t> &index);
		bool Create(std::vector<Vertex>&& mesh, std::vector<uint32_t>&& index);
		uint64_t VertexCount() const;
		uint64_t VertexSize() const;
		uint64_t IndexCount() const;
		uint64_t IndexSize() const;
		const Vertex*	Vertices() const;
		const uint32_t* Indices() const;

	private:
		std::vector<Vertex>		mMesh;
		std::vector<uint32_t>	mIndex;
	};
}
