#include "SkyBoxPass.h"

using namespace YD3D;

SkyBoxPass::SkyBoxPass()
{
}

SkyBoxPass::~SkyBoxPass()
{
}

bool SkyBoxPass::PopulateCommandList(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	return false;
}

bool SkyBoxPass::SerializeRootSignature()
{
	return false;
}

bool SkyBoxPass::BuildPSO()
{
	///*D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	//desc.InputLayout = { mInitParam.InputLayout.data(), (UINT)mInitParam.InputLayout.size() };
	//desc.pRootSignature = mRootSignature.Get();
	//desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//desc.SampleMask = UINT_MAX;
	//desc.PrimitiveTopologyType = mInitParam.PrimitiveType;
	//desc.NumRenderTargets = mInitParam.RenderTargetFormat.size();
	//desc.RTVFormats[0] = mInitParam.RenderTargetFormat[0];
	//desc.DSVFormat = mInitParam.DepthStencilFormat;
	//desc.SampleDesc.Count = 1;*/

	//if (!mArrShaderResPath[EShaderType::VS].empty())
	//{
	//	desc.VS = { mArrShaderRes[EShaderType::VS]->GetBufferPointer(), mArrShaderRes[EShaderType::VS]->GetBufferSize() };
	//}

	//if (!mArrShaderResPath[EShaderType::PS].empty())
	//{
	//	desc.PS = { mArrShaderRes[EShaderType::PS]->GetBufferPointer(), mArrShaderRes[EShaderType::PS]->GetBufferSize() };
	//}

	//if (!mArrShaderResPath[EShaderType::DS].empty())
	//{
	//	desc.DS = { mArrShaderRes[EShaderType::DS]->GetBufferPointer(), mArrShaderRes[EShaderType::DS]->GetBufferSize() };
	//}

	//if (!mArrShaderResPath[EShaderType::GS].empty())
	//{
	//	desc.GS = { mArrShaderRes[EShaderType::GS]->GetBufferPointer(), mArrShaderRes[EShaderType::GS]->GetBufferSize() };
	//}

	//if (!mArrShaderResPath[EShaderType::HS].empty())
	//{
	//	desc.HS = { mArrShaderRes[EShaderType::HS]->GetBufferPointer(), mArrShaderRes[EShaderType::HS]->GetBufferSize() };
	//}

	//ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPSO)));

	return true;
}
