#include "Model.h"

namespace YD3D 
{
	using namespace DirectX;

	Model::Model()
	{
	}

	Model::~Model() 
	{
	}

	bool Model::Create(ID3D12Device* device, const std::vector<Vertex>& mesh, const std::vector<uint32_t>& index)
	{
		mDevice = device;
		mMesh = mesh;
		mIndex = index;

		mGrpModelInfo.assign(new GraphicConstBuffer<ModelInfo, 1>);
		mGrpModelInfo->Create(mDevice);

		UpdateModelInfo();
		return true;
	}

	bool Model::Create(ID3D12Device* device, std::vector<Vertex>&& mesh, std::vector<uint32_t>&& index)
	{
		mDevice = device;
		mMesh = std::move(mesh);
		mIndex = std::move(index);

		mGrpModelInfo.assign(new GraphicConstBuffer<ModelInfo, 1>);
		mGrpModelInfo->Create(mDevice);

		UpdateModelInfo();
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

	GraphicConstBuffer<ModelInfo, 1>* Model::GraphicModelInfo()
	{
		return mGrpModelInfo.get_raw_ptr();
	}

	void Model::UpdateModelInfo()
	{
		auto&& matPos = XMMatrixTranslation(mModelInfo.Position.x, mModelInfo.Position.y, mModelInfo.Position.z);
		auto&& matRotationX = XMMatrixRotationX(mModelInfo.Rotation.x);
		auto&& matRotationY = XMMatrixRotationX(mModelInfo.Rotation.y);
		auto&& matRotationZ = XMMatrixRotationX(mModelInfo.Rotation.z);
		auto&& matScalar = XMMatrixScaling(mModelInfo.Scalar.x, mModelInfo.Scalar.y, mModelInfo.Scalar.z);

		auto&& matModel = matScalar * matRotationX * matRotationY * matRotationZ * matPos;

		XMStoreFloat4x4(&mModelInfo.ModelMatrix, matModel);
		mGrpModelInfo->Update(0, mModelInfo);
	}

}