#pragma once
#include "PipeLine/PipeLineTemplate.hpp"
#include "PipeLine/ResourcePackage.h"
#include "TestPass.h"

class TestPipeLine;

typedef YD3D::PipeLineTemplate<YD3D::ResourcePackage, YD3D::PipeLineInitParam> Super;
typedef bool (TestPipeLine::*PopulateCommandListFunction) (YD3D::ResourcePackage*, ID3D12GraphicsCommandList*);
typedef bool (TestPipeLine::* PopulateCommandListFunctionGC) (gc_ptr<YD3D::ResourcePackage>, ID3D12GraphicsCommandList*);

class TestPipeLine : public YD3D::PipeLineTemplate<YD3D::ResourcePackage, YD3D::PipeLineInitParam>
{
public:
	TestPipeLine();
	~TestPipeLine();

	bool Create(ID3D12Device* device, const YD3D::PipeLineInitParam* pipeLineInitParam) override;
	bool SetScene(YD3D::Scene* scene) override;
protected:
	bool Draw(YD3D::ResourcePackage *package) override;
	bool PopulateCommandList(YD3D::ResourcePackage *package, ID3D12GraphicsCommandList* commandlist) override;
	bool PopulateCommandListGC(gc_ptr<YD3D::ResourcePackage> package, ID3D12GraphicsCommandList* commandlist);

	bool PostToCommandQueue(YD3D::ECommandQueueType type, YD3D::ResourcePackage* package, PopulateCommandListFunction func, uint64_t* fence);
	bool PostToCommandQueue(YD3D::ECommandQueueType type, YD3D::ResourcePackage* package, PopulateCommandListFunction func, uint64_t *fence, YD3D::GraphicTaskCallbackFunction &&callback);
	bool PostToCommandQueue(YD3D::ECommandQueueType type, YD3D::ResourcePackage* package, YD3D::GraphicTaskFunction &&func, uint64_t* fence);
	bool PostToCommandQueue(YD3D::ECommandQueueType type, YD3D::ResourcePackage* package, YD3D::GraphicTaskFunction &&func, uint64_t *fence, YD3D::GraphicTaskCallbackFunction&& callback);

	bool PostToCommandQueueGC(YD3D::ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, PopulateCommandListFunctionGC func, uint64_t* fence);
	bool PostToCommandQueueGC(YD3D::ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, PopulateCommandListFunctionGC func, uint64_t* fence, YD3D::GraphicTaskCallbackFunction&& callback);
	bool PostToCommandQueueGC(YD3D::ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, YD3D::GraphicTaskFunction&& func, uint64_t* fence);
	bool PostToCommandQueueGC(YD3D::ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, YD3D::GraphicTaskFunction&& func, uint64_t* fence, YD3D::GraphicTaskCallbackFunction&& callback);

	bool WaitForCommandQueueCompletion(YD3D::ECommandQueueType type, uint64_t fenceValue, bool waitCallBack = true);

	bool PopulateBeginPipeLine(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList);
	bool PopulateEndPipeLine(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList);

	TestPass mPass;
};

