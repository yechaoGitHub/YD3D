#pragma once

#include "YD3D_Header.h"
#include "PipeLine/PassTemplate.hpp"
#include "PipeLine/ResourcePackage.h"

struct TestPassInitParam : public YD3D::PassInitParam
{
	TestPassInitParam() 
	{
		
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

