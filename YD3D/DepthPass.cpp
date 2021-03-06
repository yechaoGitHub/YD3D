#include "DepthPass.h"

using namespace YD3D;

DepthPass::DepthPass()
{
	mArrShaderResPath[EShaderType::VS] = _HLSL_FILE_PATH_ + L"DepthPassVs.hlsl";
	mArrShaderResPath[EShaderType::PS] = _HLSL_FILE_PATH_ + L"DepthPassPs.hlsl";
}

DepthPass::~DepthPass()
{

}

bool DepthPass::SerializeRootSignature()
{
	CD3DX12_ROOT_PARAMETER rootParam[3] = {};
	rootParam[0].InitAsConstantBufferView(0);
	rootParam[1].InitAsConstantBufferView(1);
	rootParam[2].InitAsConstantBufferView(2);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
		3,
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

bool DepthPass::PopulateCommandList(DepthPassRenderItem* renderItem, ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(mPSO.Get());
	commandList->SetGraphicsRootSignature(mRootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->OMSetRenderTargets(1, &renderItem->RtHandle, true, nullptr);

	for (auto& pair : mScene->GetDrawParam())
	{
		const DrawParam& drawParam = pair.second;
		Model* model = drawParam.Model;
		commandList->SetGraphicsRootConstantBufferView(0, model->GraphicModelInfo()->GetGpuAddress());
		commandList->SetGraphicsRootConstantBufferView(1, mScene->GraphicSceneInfo()->GetGpuAddress());

		commandList->DrawIndexedInstanced(drawParam.IndexCountPerInstance, 1, drawParam.StartIndexLocation, drawParam.BaseVertexLocation, drawParam.StartInstanceLocation);
	}

	return true;
}


