#pragma once

#include "YD3D_Header.h"
#include "PipeLine/PassTemplate.hpp"
#include "PipeLine/ResourcePackage.h"

struct TestPassInitParam : public YD3D::PassInitParam
{
	TestPassInitParam() 
	{
		InputLayout = YD3D::GetCommonInputLayout();
		RenderTargetFormat = { DXGI_FORMAT_R8G8B8A8_UNORM };
		DepthStencilFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
		PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
};

class TestPass : public YD3D::PassTemplate<YD3D::ResourcePackage, TestPassInitParam>
{
public:
	TestPass();
	~TestPass();

	bool PopulateCommandList(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList) override;

protected:
	bool SerializeRootSignature();
};

