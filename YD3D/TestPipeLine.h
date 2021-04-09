#pragma once
#include "PipeLine/PipeLineTemplate.hpp"
#include "PipeLine/ResourcePackage.h"
#include "PipeLine/LatLongToCubeMapPass.h"
#include "TestPass.h"
#include "DepthPass.h"

class TestPipeLine;

struct TestResourcePackage : public YD3D::ResourcePackage
{
	DepthPassRenderItem DepthItem;
	YD3D::LatLongToCubeMapRenderItem LatlongItem;
};

class TestPipeLine : public YD3D::PipeLineTemplate<TestResourcePackage, YD3D::PipeLineInitParam>
{
	typedef YD3D::PipeLineTemplate<TestResourcePackage, YD3D::PipeLineInitParam> Super;
public:
	TestPipeLine();
	~TestPipeLine();

	bool Create(ID3D12Device* device, const YD3D::PipeLineInitParam* pipeLineInitParam) override;
	bool SetScene(YD3D::Scene* scene) override;
	bool Draw(TestResourcePackage*package) override;

protected:
	bool PopulateCommandList(TestResourcePackage* package, ID3D12GraphicsCommandList* commandlist) override;
	bool PopulateCommandListGC(gc_ptr<TestResourcePackage> package, ID3D12GraphicsCommandList* commandlist) override;

	bool PopulateBeginPipeLine(TestResourcePackage* package, ID3D12GraphicsCommandList* commandList);
	bool PopulateEndPipeLine(TestResourcePackage* package, ID3D12GraphicsCommandList* commandList);

	TestPass	mPass;
	DepthPass	mDepthPass;

	YD3D::LatLongToCubeMapPass mLatLongPass;

};

