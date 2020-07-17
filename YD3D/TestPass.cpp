#include "TestPass.h"
#include "Graph/DescriptorHeapManager.h"

using namespace YD3D;

const WCHAR* _VS_HLSL_NAME_ = L"CommonVs.hlsl";
const WCHAR* _PS_HLSL_NAME_ = L"ShapePs.hlsl";

TestPass::TestPass()
{
	mArrShaderResPath[EShaderType::VS] = _HLSL_FILE_PATH_ + L"CommonVs.hlsl";
	mArrShaderResPath[EShaderType::PS] = _HLSL_FILE_PATH_ + L"ShapePs.hlsl";
}

TestPass::~TestPass()
{

}

bool TestPass::PopulateCommandList(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(mPSO.Get());
	commandList->SetGraphicsRootSignature(mRootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->OMSetRenderTargets(1, &package->RtHandle, true, &package->DsHandle);

	ID3D12DescriptorHeap *descHeap = DescriptorHeapManager::GobalDescriptorHeapManager()->GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->Descriptor();
	ID3D12DescriptorHeap* descriptorHeaps[] = { descHeap };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	for (auto& pair : mScene->GetDrawParam())
	{
		const DrawParam& drawParam = pair.second;
		Model *model = drawParam.Model;

		commandList->SetGraphicsRootDescriptorTable(0, model->GraphicResource().mTextures[0]->GetGpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 0));
		commandList->SetGraphicsRootConstantBufferView(1, model->GraphicModelInfo()->GetGpuAddress());
		commandList->SetGraphicsRootConstantBufferView(2, mScene->GraphicSceneInfo()->GetGpuAddress());
	
		commandList->DrawIndexedInstanced(drawParam.IndexCountPerInstance, 1, drawParam.StartIndexLocation, drawParam.BaseVertexLocation, drawParam.StartInstanceLocation);
	}

	return true;
}

bool TestPass::SerializeRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParam[4] = {};
	CD3DX12_DESCRIPTOR_RANGE range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0);

	rootParam[0].InitAsDescriptorTable(1, &range);
	rootParam[1].InitAsConstantBufferView(0);
	rootParam[2].InitAsConstantBufferView(1);
	rootParam[3].InitAsConstantBufferView(2);
	

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		4,
		rootParam,
		0,
		nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &mSerializedRootSignature, &errorBlob));

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	return true;
}

