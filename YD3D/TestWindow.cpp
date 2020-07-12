#include "TestWindow.h"

using namespace YD3D;

ID3D12Device* TestWindow::_D3D12DEVICE_(nullptr);

TestWindow::TestWindow()
{

}

TestWindow::~TestWindow()
{

}

bool TestWindow::Create(uint32_t width, uint32_t height, const std::wstring& caption)
{
	YWindow::Create(width, height);
	InitD3D();
	InitScence();
	InitResource();

	return true;
}

uint32_t TestWindow::Run()
{
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (mPackage->State.is_state(EResourcePackageState::ERENDERED)) 
			{
				mSwapChain.Present(0, 0);
				mPackage->State.set_state(EResourcePackageState::EINIT);
				mPackage->RT = get_gc_ptr_from_raw(mSwapChain.GetCurBackBuffer());
				mPipeLine.PostResourcePackage(mPackage.get_raw_ptr());
			}
		}
	}

	return (int)msg.wParam;
}

LRESULT TestWindow::MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(GetHandle(), msg, wParam, lParam);
}

void TestWindow::InitD3D()
{
	assert(mDxFactory.Create());
	_D3D12DEVICE_ = mDxFactory.Device();

	assert(mGraphTask.Create(mDxFactory.Device()));
	GraphicTask::SetGlobalGraphicTask(mGraphTask);

	assert(mDescriptorManager.Create(mDxFactory.Device(), 20));
	DescriptorHeapManager::SetGobalDescriptorHeapManager(mDescriptorManager);

	assert(mSwapChain.Create(mDxFactory.Factory(), mDxFactory.Device(), GetHandle(), GetWidth(), GetHeight()));

	PipeLineInitParam initParam;
	initParam.outputWidth = GetWidth();
	initParam.outputHeight = GetHeight();
	assert(mPipeLine.Create(mDxFactory.Device(), &initParam));
}

void TestWindow::InitScence()
{	std::vector<YD3D::Vertex> vecVertex =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(+1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f),

		Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f, +1.0f, +1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f),

		Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, +1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f),

		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		Vertex(-1.0f, -1.0f, +1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f),

		Vertex(-1.0f, -1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f),
		Vertex(-1.0f, +1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f),
		Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f),

		Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f)
	};

	std::vector<uint32_t> vecIndex =
	{
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7,
		8, 9, 10,
		8, 10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23,
	};

	gc_ptr<Model> model;
	model.assign(new Model);
	mScene.assign(new Scene);

	model->Create(vecVertex, vecIndex);
	mScene->Create(_D3D12DEVICE_);
	mScene->AddModel(model);
	mScene->UpdateGraphicResource(true);
}

void TestWindow::InitResource()
{
	mDs.assign(new GraphicDepthStencil);
	mDs->Create(_D3D12DEVICE_, GetWidth(), GetHeight());
	for (uint32_t i = 0; i < 2; i++) 
	{
		DescriptorHeapManager::GobalDescriptorHeapManager()->BindRtView(ANY_DESCRIPTOR_HEAP_POS, mSwapChain.GetBackBuffer(i), nullptr);
	}
	DescriptorHeapManager::GobalDescriptorHeapManager()->BindDsView(ANY_DESCRIPTOR_HEAP_POS, mDs.get_raw_ptr(), nullptr);

	mPackage.assign(new ResourcePackage);
	mPackage->RT = get_gc_ptr_from_raw(mSwapChain.GetCurBackBuffer());
	mPackage->DS = mDs;
	mPackage->State.set_state(EResourcePackageState::EINIT);

	mPipeLine.PostResourcePackage(mPackage.get_raw_ptr());
}

