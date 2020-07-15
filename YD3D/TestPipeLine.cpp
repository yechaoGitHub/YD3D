#include "TestPipeLine.h"
#include "Graph/DescriptorHeapManager.h"

using namespace YD3D;

TestPipeLine::TestPipeLine() 
{

}

TestPipeLine::~TestPipeLine()
{

}

bool TestPipeLine::Create(ID3D12Device* device, const YD3D::PipeLineInitParam* pipeLineInitParam) 
{
	TestPassInitParam passInitParam;
	mPass.Create(device, &passInitParam);
	Super::Create(device, pipeLineInitParam);

	return true;
}

bool TestPipeLine::SetScene(Scene* scene)
{
	mScene = get_gc_ptr_from_raw(scene);
	mPass.SetScene(scene);
	return true;
}

bool TestPipeLine::Draw(YD3D::ResourcePackage *package)
{
	uint64_t feneValue(0);

	package->State.set_state(EResourcePackageState::ERENDERING);

	ResetCommand();
	PopulateCommandList(package, mCommandList.Get());
	mCommandList->Close();

	GraphicTask::GlobalGraphicTask()->GetCommandQueue(ECommandQueueType::ESWAP_CHAIN).PostCommandList(1, mCommandList.GetAddressOf(), &feneValue);
	GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ESWAP_CHAIN, feneValue, false);

	package->State.set_state(EResourcePackageState::ERENDERED);
	
	return true;
}


bool TestPipeLine::PopulateCommandList(YD3D::ResourcePackage *package, ID3D12GraphicsCommandList* commandList)
{
	PopulateBeginPipeLine(package, commandList);
	mPass.PopulateCommandList(package, commandList);
	PopulateEndPipeLine(package, commandList);

	return true;
}

bool TestPipeLine::PopulateCommandListGC(gc_ptr<YD3D::ResourcePackage> package, ID3D12GraphicsCommandList* commandList)
{
	PopulateBeginPipeLine(package.get_raw_ptr(), commandList);
	mPass.PopulateCommandList(package.get_raw_ptr(), commandList);
	PopulateEndPipeLine(package.get_raw_ptr(), commandList);

	return true;
}

bool TestPipeLine::PostToCommandQueue(ECommandQueueType type, YD3D::ResourcePackage* package, PopulateCommandListFunction func, uint64_t* fence)
{
	GraphicTaskFunction&& task = std::bind(func, this, package, std::placeholders::_1);
	return GraphicTask::PostGraphicTask(type, std::move(task), fence);
}

bool TestPipeLine::PostToCommandQueue(ECommandQueueType type, YD3D::ResourcePackage* package, PopulateCommandListFunction func, uint64_t* fence, GraphicTaskCallbackFunction&& callback)
{
	GraphicTaskFunction&& task = std::bind(func, this, package, std::placeholders::_1);
	return GraphicTask::PostGraphicTask(type, std::move(task), fence, std::move(callback));
}

bool TestPipeLine::PostToCommandQueue(ECommandQueueType type, YD3D::ResourcePackage* package, GraphicTaskFunction&& func, uint64_t* fence)
{
	return GraphicTask::PostGraphicTask(type, std::move(func), fence);
}

bool TestPipeLine::PostToCommandQueue(ECommandQueueType type, YD3D::ResourcePackage* package, GraphicTaskFunction&& func, uint64_t* fence, GraphicTaskCallbackFunction&& callback)
{
	return GraphicTask::PostGraphicTask(type, std::move(func), fence, std::move(callback));
}

bool TestPipeLine::PostToCommandQueueGC(ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, PopulateCommandListFunctionGC func, uint64_t* fence)
{
	GraphicTaskFunction&& task = std::bind(func, this, package, std::placeholders::_1);
	return 	GraphicTask::PostGraphicTask(type, std::move(task), fence);
}

bool TestPipeLine::PostToCommandQueueGC(ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, PopulateCommandListFunctionGC func, uint64_t* fence, GraphicTaskCallbackFunction&& callback)
{
	GraphicTaskFunction&& task = std::bind(func, this, package, std::placeholders::_1);
	return GraphicTask::PostGraphicTask(type, std::move(task), fence, std::move(callback));
}

bool TestPipeLine::PostToCommandQueueGC(ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, GraphicTaskFunction&& func, uint64_t* fence)
{
	return GraphicTask::PostGraphicTask(type, std::move(func), fence);
}

bool TestPipeLine::PostToCommandQueueGC(ECommandQueueType type, gc_ptr<YD3D::ResourcePackage> package, GraphicTaskFunction&& func, uint64_t* fence, GraphicTaskCallbackFunction&& callback)
{
	return GraphicTask::PostGraphicTask(type, std::move(func), fence, std::move(callback));
}

bool TestPipeLine::PopulateBeginPipeLine(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &mViewport);
	commandList->RSSetScissorRects(1, &mScissorRect);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));

	commandList->ClearRenderTargetView(package->RtHandle, DirectX::Colors::Brown, 0, nullptr);
	commandList->ClearDepthStencilView(package->DsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	commandList->IASetVertexBuffers(0, 1, &mScene->VertexView());
	commandList->IASetIndexBuffer(&mScene->IndexView());

	return true;
}

bool TestPipeLine::PopulateEndPipeLine(YD3D::ResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));
	return true;
}

