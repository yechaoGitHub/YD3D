#include "TestPipeLine.h"
#include "Graph/DescriptorHeapManager.h"

using namespace YD3D;

TestPipeLine::TestPipeLine() 
{

}

TestPipeLine::~TestPipeLine()
{

}

bool TestPipeLine::Draw(YD3D::ResourcePackage *package)
{
	package->State.set_state(EResourcePackageState::ERENDERING);
	return PostToCommandQueue(package);
}

bool TestPipeLine::PopulateCommandList(YD3D::ResourcePackage *package, ID3D12GraphicsCommandList* commandList)
{
	DescriptorHeapManager *descriptor = DescriptorHeapManager::GobalDescriptorHeapManager();
	D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = descriptor->GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, package->RT.get_raw_ptr());
	assert(IsNullCpuDescriptorHandle(rtHandle));
	D3D12_CPU_DESCRIPTOR_HANDLE dsHandle = descriptor->GetResourceCpuHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, package->DS.get_raw_ptr());
	assert(IsNullCpuDescriptorHandle(dsHandle));

	commandList->RSSetViewports(1, &mViewport);
	commandList->RSSetScissorRects(1, &mScissorRect);

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET));

	commandList->ClearRenderTargetView(rtHandle, DirectX::Colors::Brown, 0, nullptr);
	commandList->ClearDepthStencilView(dsHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	commandList->IASetVertexBuffers(0, 1, &mScene->VertexView());
	commandList->IASetIndexBuffer(&mScene->IndexView());

	for (auto &pair : mScene->GetDrawParam()) 
	{
		const DrawParam &drawParam = pair.second;
		commandList->DrawIndexedInstanced(drawParam.IndexCountPerInstance, 1, drawParam.StartIndexLocation, drawParam.BaseVertexLocation, drawParam.StartInstanceLocation);
	}

	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(package->RT->Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON));

	return true;
}
