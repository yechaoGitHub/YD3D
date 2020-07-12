#pragma once
#include "PipeLine/PipeLineTemplate.hpp"
#include "PipeLine/ResourcePackage.h"

class TestPipeLine : public YD3D::PipeLineTemplate<YD3D::ResourcePackage, YD3D::PipeLineInitParam>
{
public:
	TestPipeLine();
	~TestPipeLine();

protected:
	bool Draw(YD3D::ResourcePackage *package) override;
	bool PopulateCommandList(YD3D::ResourcePackage *package, ID3D12GraphicsCommandList* commandlist) override;
};
