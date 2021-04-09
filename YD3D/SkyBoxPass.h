#pragma once
#include "YD3D_Header.h"
#include "PipeLine/PassTemplate.hpp"
#include "PipeLine/ResourcePackage.h"

struct SkyBoxPassInitParam : public YD3D::PassInitParam
{
	SkyBoxPassInitParam()
	{
		
	}
};

class SkyBoxPass : public YD3D::PassTemplate<YD3D::ResourcePackage, SkyBoxPassInitParam>
{
public:
	SkyBoxPass();
	~SkyBoxPass();

	bool PopulateCommandList(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList) override;

protected:
	bool SerializeRootSignature();
	bool BuildPSO();
};

