#include "Model.h"

namespace YD3D 
{
	Model::Model()
	{
	}

	Model::~Model() 
	{
	}

	bool Model::Create(const std::vector<Vertex>& mesh, const std::vector<uint32_t>& index)
	{
		mMesh = mesh;
		mIndex = index;
		return true;
	}

	bool Model::Create(std::vector<Vertex>&& mesh, std::vector<uint32_t>&& index)
	{
		mMesh = std::move(mesh);
		mIndex = std::move(index);
		return true;
	}


	uint64_t Model::VertexCount() const
	{
		return mMesh.size();
	}

	uint64_t Model::VertexSize() const
	{
		return mMesh.size() * sizeof(Vertex);
	}

	uint64_t Model::IndexCount() const
	{
		return mIndex.size();
	}

	uint64_t Model::IndexSize() const
	{
		return mIndex.size() * sizeof(uint32_t);
	}

	const Vertex* Model::Vertices() const
	{
		return mMesh.data();
	}

	const uint32_t* Model::Indices() const
	{
		return mIndex.data();
	}

}