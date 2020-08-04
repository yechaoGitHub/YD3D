#include "LatLongToCubeMapPass.h"
#include "Util/YD3D_Define.h"
#include "Graph/GraphicTask.h"
#include "Graph/DescriptorHeapManager.h"
#include "Helper/GeometricMeshFactory.h"
#include "Util/CommonVertexIndexBuffer.h"

using namespace DirectX;

namespace YD3D
{
	LatLongToCubeMapPass::LatLongToCubeMapPass() :
		mPreIndexCount(0),
		mWidth(0),
		mHeight(0)
	{
		mArrShaderResPath[EShaderType::VS] = _HLSL_FILE_PATH_ + L"LatLongToCubeMapVs.hlsl";
		mArrShaderResPath[EShaderType::PS] = _HLSL_FILE_PATH_ + L"LatLongToCubeMapPs.hlsl";
	}

	LatLongToCubeMapPass::~LatLongToCubeMapPass()
	{
	}

	bool LatLongToCubeMapPass::Create(ID3D12Device* device, const LatLongToCubeMapInitParam* initParam)
	{
		Super::Create(device, initParam);
		mCamera.SetLens(0.5 * Pi, 1, 0.1f, 10.0f);
		mCbBuffer.Create(device, 1);

		MeshData &&meshData = GeometricMeshFactory::CreateBox(XMFLOAT3(1.0f, 1.0f, 1.0f), true);
		uint64_t vertexSize = meshData.Vertices.size() * sizeof(Vertex);
		uint64_t indexSize = meshData.Indices.size() * sizeof(uint32_t);
		uint64_t totalSize = vertexSize + indexSize;

		mPreIndexCount = meshData.Indices.size();

		mUploadBuffer.Create(device, totalSize * 1.5);
		mVertexBuffer.Create(device, sizeof(Vertex), meshData.Vertices.size());
		mIndexBuffer.Create(device, indexSize, DXGI_FORMAT_R32_UINT);

		mVertexBuffer.SetName(L"LatLongVS");
		mIndexBuffer.SetName(L"LatLongIndex");

		mUploadBuffer.CopyData(0, reinterpret_cast<const uint8_t*>(meshData.Vertices.data()), vertexSize);
		mUploadBuffer.CopyData(vertexSize, reinterpret_cast<const uint8_t*>(meshData.Indices.data()), indexSize);

		InitViewProjMatrix();

		uint64_t fenceValue = GraphicTask::PostGraphicTask(
			ECommandQueueType::ECOPY,
			[this, vertexSize, indexSize](ID3D12GraphicsCommandList* commandList)
			{

				CD3DX12_RESOURCE_BARRIER barrier[4];

				barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
				barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(mIndexBuffer.Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
				barrier[2] = CD3DX12_RESOURCE_BARRIER::Transition(mVertexBuffer.Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);
				barrier[3] = CD3DX12_RESOURCE_BARRIER::Transition(mIndexBuffer.Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_COMMON);

				commandList->ResourceBarrier(2, &barrier[0]);

				commandList->CopyBufferRegion(mVertexBuffer.Resource(), 0, mUploadBuffer.Resource(), 0, vertexSize);
				commandList->CopyBufferRegion(mIndexBuffer.Resource(), 0, mUploadBuffer.Resource(), vertexSize, indexSize);

				commandList->ResourceBarrier(2, &barrier[2]);
			});

		GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue);

		return true;
	}

	void LatLongToCubeMapPass::SetViewProjIndex(ECubeCameraDirection direction)
	{
		LatLongToCubeMapCbBuffer* cbBuffer = reinterpret_cast<LatLongToCubeMapCbBuffer*>(mCbBuffer.GetMappedPointer());
		cbBuffer->MatrixIndex = direction;
	}

	void LatLongToCubeMapPass::SetSize(uint32_t width, uint32_t height)
	{
		mWidth = width;
		mHeight = height;
	}

	void LatLongToCubeMapPass::InitViewProjMatrix()
	{
		LatLongToCubeMapCbBuffer* cbBuffer = reinterpret_cast<LatLongToCubeMapCbBuffer*>(mCbBuffer.GetMappedPointer());

		//RIGHT:
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[0], mCamera.GetView() * mCamera.GetProj());

		// LEFT
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[1], mCamera.GetView() * mCamera.GetProj());

		// TOP
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[2], mCamera.GetView() * mCamera.GetProj());

		// BOTTOM
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[3], mCamera.GetView() * mCamera.GetProj());

		// BACK
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[4], mCamera.GetView() * mCamera.GetProj());

		// FRONT
		mCamera.LookAt(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f));
		mCamera.UpdateViewMatrix();
		XMStoreFloat4x4(&cbBuffer->ViewProjMatrix[5], mCamera.GetView() * mCamera.GetProj());

		cbBuffer->MatrixIndex = 0;
	}

	bool LatLongToCubeMapPass::PopulateCommandList(LatLongToCubeMapRenderItem* package, ID3D12GraphicsCommandList* commandList)
	{
		return PopulateCommandList(package->LatLongTexture, package->RenderTarget, package->DepthStencil, commandList);
	}

	bool LatLongToCubeMapPass::PopulateCommandList(GraphicTexture* latLongTexture, GraphicRenderTarget* renderTarget, GraphicDepthStencil* depthStencil, ID3D12GraphicsCommandList* commandList)
	{
		ID3D12DescriptorHeap* heap[] = { DescriptorHeapManager::GobalDescriptorHeapManager()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Descriptor() };
		D3D12_GPU_DESCRIPTOR_HANDLE LatLongTexhandle = latLongTexture->GetGpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = renderTarget->GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_VIEWPORT viewPort = { 0.0f, 0.0f, static_cast<float>(mWidth), static_cast<float>(mHeight), 0.0f, 1.0f };
		D3D12_RECT rectangle = { 0, 0, mWidth, mHeight };


		commandList->RSSetViewports(1, &viewPort);
		commandList->RSSetScissorRects(1, &rectangle);
		commandList->SetPipelineState(mPSO.Get());
		commandList->SetGraphicsRootSignature(mRootSignature.Get());
		commandList->SetDescriptorHeaps(1, heap);

		commandList->OMSetRenderTargets(1, &rtHandle, true, &depthStencil->GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
		commandList->ClearDepthStencilView(depthStencil->GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
		
		commandList->IASetVertexBuffers(0, 1, &mVertexBuffer.VertexView());
		commandList->IASetIndexBuffer(&mIndexBuffer.IndexView());

		commandList->SetGraphicsRootDescriptorTable(0, LatLongTexhandle);
		commandList->SetGraphicsRootConstantBufferView(1, mCbBuffer.GetGpuAddress());
		commandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));
		commandList->ClearRenderTargetView(rtHandle, DirectX::Colors::LightBlue, 0, nullptr);
		commandList->DrawIndexedInstanced(mPreIndexCount, 1, 0, 0, 0);
		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));

		return true;
	}

	bool LatLongToCubeMapPass::SerializeRootSignature()
	{
		CD3DX12_ROOT_PARAMETER rootParam[2] = {};
		CD3DX12_DESCRIPTOR_RANGE range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		rootParam[0].InitAsDescriptorTable(1, &range);
		rootParam[1].InitAsConstantBufferView(0);

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
			2,
			rootParam,
			GetStaticSamplers().size(),
			GetStaticSamplers().data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &mSerializedRootSignature, &errorBlob));

		if (errorBlob != nullptr)
		{
			::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}

		return true;
	}
};
