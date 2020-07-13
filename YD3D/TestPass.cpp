#include "TestPass.h"

using namespace YD3D;

const WCHAR* _VS_HLSL_NAME_ = L"CommonVs.hlsl";
const WCHAR* _PS_HLSL_NAME_ = L"ShapePs.hlsl";

TestPass::TestPass()
{

}

TestPass::~TestPass()
{

}

bool TestPass::Create(ID3D12Device* device, const TestPassInitParam* initParam)
{
	mDevice = device;
	mInitParam = *initParam;

	SerializeRootSignature();
	BuildRootSignature();
	BuildShaderResource();
	BuildPSO();

	return true;
}

bool TestPass::PopulateCommandList(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(mPSO.Get());
	commandList->SetGraphicsRootSignature(mRootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->OMSetRenderTargets(1, &package->RtHandle, true, &package->DsHandle);

	for (auto& pair : mScene->GetDrawParam())
	{
		const DrawParam& drawParam = pair.second;
		Model *model = drawParam.Model;
		commandList->SetGraphicsRootConstantBufferView(0, mScene->GraphicSceneInfo()->GetGpuAddress());
		commandList->SetGraphicsRootConstantBufferView(1, model->GraphicModelInfo()->GetGpuAddress());

		commandList->DrawIndexedInstanced(drawParam.IndexCountPerInstance, 1, drawParam.StartIndexLocation, drawParam.BaseVertexLocation, drawParam.StartInstanceLocation);
	}

	return true;
}

bool TestPass::SerializeRootSignature()
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

bool TestPass::BuildRootSignature()
{
	void* serializedBlob = mSerializedRootSignature->GetBufferPointer();
	UINT serializedSize = mSerializedRootSignature->GetBufferSize();
	ThrowIfFailed(mDevice->CreateRootSignature(0, serializedBlob, serializedSize, IID_PPV_ARGS(&mRootSignature)));
	return true;
}

bool TestPass::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = { mInitParam.InputLayout.data(), (UINT)mInitParam.InputLayout.size() };
	desc.pRootSignature = mRootSignature.Get();
	desc.VS = { mArrShaderRes[EShaderType::VS]->GetBufferPointer(), mArrShaderRes[EShaderType::VS]->GetBufferSize() };
	desc.PS = { mArrShaderRes[EShaderType::PS]->GetBufferPointer(), mArrShaderRes[EShaderType::PS]->GetBufferSize() };
	desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = mInitParam.PrimitiveType;
	desc.NumRenderTargets = mInitParam.RenderTargetFormat.size();
	desc.RTVFormats[0] = mInitParam.RenderTargetFormat[0];
	desc.DSVFormat = mInitParam.DepthStencilFormat;
	desc.SampleDesc.Count = 1;

	ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPSO)));

	return true;
}

bool TestPass::BuildShaderResource()
{
	std::wstring VsPath = _HLSL_FILE_PATH_ + std::wstring(_VS_HLSL_NAME_);
	std::wstring PsPath = _HLSL_FILE_PATH_ + std::wstring(_PS_HLSL_NAME_);

	mArrShaderRes[EShaderType::VS] = CompileShader(VsPath.c_str(), nullptr, "VS", "vs_5_1");
	mArrShaderRes[EShaderType::PS] = CompileShader(PsPath.c_str(), nullptr, "PS", "ps_5_1");

	return true;
}

