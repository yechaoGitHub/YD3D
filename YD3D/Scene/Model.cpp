#include "Model.h"

namespace YD3D 
{
	using namespace DirectX;

	Model::Model():
		mDevice(nullptr),
		mTextureCount(0)
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

	ModelGraphicResource& Model::GraphicResource()
	{
		return mGpuResource;
	}

	void Model::UpdateTexture(YD3D::WICImage *images, uint32_t count)
	{
		assert(count <= 8);

		mTextureCount = count;

		uint64_t totalSize(0);
		for (uint32_t i = 0; i < count; i++)
		{
			totalSize += images[i].Size();
		}

		if ((!mGpuResource.mUploader) || (mGpuResource.mUploader->GetResByteSize() < totalSize))
		{
			mGpuResource.mUploader.assign(new GraphicUploadBuffer);
			mGpuResource.mUploader->Create(mDevice, totalSize * 4);
		}

		uint64_t offset(0);
		for (uint32_t i = 0; i < count; i++)
		{
			uint32_t &&imageSize = images[i].Size();

			mTextureLayouts[i].Length = imageSize;
			mTextureLayouts[i].Offset = offset;

			if ((!mGpuResource.mTextures[i]) || (mGpuResource.mTextures[i]->GetResByteSize() < imageSize))
			{
				mGpuResource.mTextures[i].assign(new GraphicTexture);
				mGpuResource.mTextures[i]->Create(mDevice, images->Width(), images->Height());
			}

			CopyableFootPrint footPrint;
			mGpuResource.mTextures[i]->GetFootPrint(footPrint, offset);

			mGpuResource.mUploader->CopyData(footPrint.footPrints[0].Offset, images[i].Data(), imageSize);
			offset += imageSize;
		}
	}

	void Model::UpdateGraphicResource(ID3D12GraphicsCommandList* commandList)
	{
		CD3DX12_RESOURCE_BARRIER beforeBarrier[8] = {};
		CD3DX12_RESOURCE_BARRIER afterBarrier[8] = {};

		for (uint32_t i = 0; i < mTextureCount; i++)
		{
			beforeBarrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(mGpuResource.mTextures[i]->Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
			afterBarrier[i] = CD3DX12_RESOURCE_BARRIER::Transition(mGpuResource.mTextures[i]->Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
		}

		commandList->ResourceBarrier(mTextureCount, beforeBarrier);

		uint64_t uploadOffset(0);
		for (uint32_t i = 0; i < mTextureCount; i++)
		{
			CopyableFootPrint footPrint;
			mGpuResource.mTextures[i]->GetFootPrint(footPrint, mTextureLayouts[i].Offset);

			CD3DX12_TEXTURE_COPY_LOCATION src(mGpuResource.mUploader->Resource(), footPrint.footPrints[0]);
			CD3DX12_TEXTURE_COPY_LOCATION dest(mGpuResource.mTextures[i]->Resource());
			
			commandList->CopyTextureRegion(&dest, 0, 0, 0, &src, nullptr);
		}
	
		commandList->ResourceBarrier(mTextureCount, afterBarrier);
	}
}
