#include "LatLongToCubeMap.h"
#include "Graph/GraphicTask.h"
#include "Helper/UploadHelper.h"
#include "Graph/DescriptorHeapManager.h"

namespace YD3D
{
	LatLongToCubeMap::LatLongToCubeMap() :
		mDevice(nullptr),
		mWidth(0),
		mHeight(0)
	{
	}

	LatLongToCubeMap::~LatLongToCubeMap()
	{
	}

	bool LatLongToCubeMap::GenerateCubeMap(ID3D12Device* device, const WICImage& latLongImage, uint32_t width, uint32_t height)
	{
		mDevice = device;
		SetUp(latLongImage, width, height);
		
		uint64_t fenceValue = GraphicTask::PostGraphicTask(ECommandQueueType::ERENDER, &LatLongToCubeMap::LatLongToCubeMapTask, this, std::placeholders::_1);
		GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ERENDER, fenceValue);

		uint8_t *data = mReadbackBuffer.Map(0, nullptr);



		mReadbackBuffer.Unmap(0, nullptr);


		ClearUp();
		return true;
	}

	void LatLongToCubeMap::SetUp(const WICImage& latLongImage, uint32_t width, uint32_t height)
	{
		if (!mPass)
		{
			mPass.Create(mDevice);
		}

		uint64_t imageSize = latLongImage.Size();

		if (!mUploadBuffer || imageSize > mUploadBuffer.GetResByteSize())
		{
			if (mUploadBuffer) mUploadBuffer.Release();
			mUploadBuffer.Create(mDevice, imageSize * 1.5);
		}

		if (!mReadbackBuffer || imageSize > mReadbackBuffer.GetResByteSize()) 
		{
			if (mReadbackBuffer) mReadbackBuffer.Release();
			mReadbackBuffer.Create(mDevice, imageSize * 1.5);
		}

		if (!mLatLongTexture || latLongImage.Size() > mLatLongTexture.GetResByteSize())
		{
			if (mLatLongTexture) mLatLongTexture.Release();
			mLatLongTexture.Create(mDevice, latLongImage.Width(), latLongImage.Height());
			DescriptorHeapManager::GobalDescriptorHeapManager()->BindSrView(ANY_DESCRIPTOR_HEAP_POS, &mLatLongTexture, nullptr);
		}

		uint64_t renderTargetSize = width * height * sizeof(uint32_t);
		if (!mRendetTarget || renderTargetSize > mRendetTarget.GetResByteSize())
		{
			if (mRendetTarget) mRendetTarget.Release();
			mRendetTarget.Create(mDevice, width, height);
			DescriptorHeapManager::GobalDescriptorHeapManager()->BindRtView(ANY_DESCRIPTOR_HEAP_POS, &mRendetTarget, nullptr);
		}

		uint64_t cubeMapSize = width * height * sizeof(uint32_t) * 6;
		if (!mCubeMapTexture || cubeMapSize > mCubeMapTexture.GetResByteSize())
		{
			if (mCubeMapTexture) mCubeMapTexture.Release();
			mCubeMapTexture.Create(mDevice, width, height, 6);
		}

		mUploadBuffer.CopyData(0, latLongImage.Data(), latLongImage.Size());
	}

	void LatLongToCubeMap::LatLongToCubeMapTask(ID3D12GraphicsCommandList* commandList)
	{
		ID3D12Resource *latLongResource = mLatLongTexture.Resource();

		CD3DX12_TEXTURE_COPY_LOCATION uploadSrc(mUploadBuffer.Resource(), GetResourceCopyableFootPrint(latLongResource, 0));
		CD3DX12_TEXTURE_COPY_LOCATION uploadDest(latLongResource, 0);
		
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(latLongResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		commandList->CopyTextureRegion(&uploadDest, 0, 0, 0, &uploadSrc, nullptr);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(latLongResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON));

	/*	mPass.SetViewProjIndex(EFRONT);
		mPass.PopulateCommandList(&mLatLongTexture, &mRendetTarget, commandList);*/

		ID3D12Resource* rtResource = mRendetTarget.Resource();
		CD3DX12_TEXTURE_COPY_LOCATION readbackSrc(rtResource, 0);
		CD3DX12_TEXTURE_COPY_LOCATION readbackDest(mReadbackBuffer.Resource(), GetResourceCopyableFootPrint(latLongResource, 0));
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtResource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_SOURCE));
		commandList->CopyTextureRegion(&readbackDest, 0, 0, 0, &readbackSrc, nullptr);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtResource, D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COMMON));

	}

	void LatLongToCubeMap::ClearUp()
	{
		DescriptorHeapManager::GobalDescriptorHeapManager()->RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, &mLatLongTexture);
		DescriptorHeapManager::GobalDescriptorHeapManager()->RemoveView(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, &mRendetTarget);
	}

};
