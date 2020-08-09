#include "TestPipeLine.h"
#include "Graph/DescriptorHeapManager.h"
#include "Util/CommonVertexIndexBuffer.h"

using namespace YD3D;

TestPipeLine::TestPipeLine() 
{

}

TestPipeLine::~TestPipeLine()
{

}

bool TestPipeLine::Create(ID3D12Device* device, const YD3D::PipeLineInitParam* pipeLineInitParam) 
{
	mPass.Create(device);
	mDepthPass.Create(device);
	mLatLongPass.Create(device);
	Super::Create(device, pipeLineInitParam);

	return true;
}

bool TestPipeLine::SetScene(Scene* scene)
{
	mScene = get_gc_ptr_from_raw(scene);
	mPass.SetScene(scene);
	mDepthPass.SetScene(scene);
	return true;
}

bool TestPipeLine::Draw(TestResourcePackage *package)
{
	package->State.set_state(EResourcePackageState::ERENDERING);
	
	ResetCommand();
	PopulateCommandList(package, mCommandList.Get());
	mCommandList->Close();

	uint64_t feneValue = GraphicTask::GlobalGraphicTask()->GetCommandQueue(ECommandQueueType::ESWAP_CHAIN).PostCommandList(1, mCommandList.GetAddressOf());
	GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ESWAP_CHAIN, feneValue, false);

	package->State.set_state(EResourcePackageState::ERENDERED);
	
	return true;
}

bool TestPipeLine::PopulateCommandList(TestResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	//PopulateBeginPipeLine(package, commandList);
	//mPass.PopulateCommandList(package, commandList);
	//mDepthPass.PopulateCommandList(&package->DepthItem, commandList);

	mLatLongPass.SetSize(1024, 1024);
	mLatLongPass.SetViewProjIndex(ELEFT);
	mLatLongPass.PopulateCommandList(&package->LatlongItem, commandList);

	//PopulateEndPipeLine(package, commandList);

	return true;
}

bool TestPipeLine::PopulateCommandListGC(gc_ptr<TestResourcePackage> package, ID3D12GraphicsCommandList* commandList)
{
	PopulateBeginPipeLine(package.get_raw_ptr(), commandList);
	//mPass.PopulateCommandList(package.get_raw_ptr(), commandList);
	//.SetSize(1024, 1024);
	//mLatLongPass.SetViewProjIndex(EFRONT);
	PopulateEndPipeLine(package.get_raw_ptr(), commandList);

	return true;
}

bool TestPipeLine::PopulateBeginPipeLine(TestResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &mViewport);
	commandList->RSSetScissorRects(1, &mScissorRect);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));

	commandList->ClearRenderTargetView(package->RtHandle, DirectX::Colors::LightBlue, 0, nullptr);
	commandList->ClearDepthStencilView(package->DsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	commandList->IASetVertexBuffers(0, 1, &mScene->VertexView());
	commandList->IASetIndexBuffer(&mScene->IndexView());

	return true;
}

bool TestPipeLine::PopulateEndPipeLine(TestResourcePackage* package, ID3D12GraphicsCommandList* commandList)
{
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));
	return true;
}

