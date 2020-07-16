#include "TestWindow.h"

using namespace YD3D;
using namespace DirectX;
ID3D12Device* TestWindow::_D3D12DEVICE_(nullptr);

TestWindow::TestWindow():
	mPreTick(0),
	mPreFPSTick(0),
	mSecondTick(0),
	mPreSecondTick(0)
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

	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//else
		//{
		//	mTimer.Tick();
		//	CalculateFrameStats();

		//	//mSwapChain.Present(0, 0);
		//	mPipeLine.Draw(mPackage.get_raw_ptr());

		//}
	}

	return (int)msg.wParam;
}

void TestWindow::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wstring fpsStr = std::to_wstring(fps);
		std::wstring mspfStr = std::to_wstring(mspf);

		std::wstring windowText = L"    fps: " + fpsStr + L"   mspf: " + mspfStr;

		SetWindowText(GetHandle(), windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void TestWindow::OnMouseMove(YD3D::EMouseKey btnState, uint32_t x, uint32_t y)
{
	if ((static_cast<uint64_t>(btnState) & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>((int32_t)x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>((int32_t)y - mLastMousePos.y));

		if (mScene->State.null_and_add(ESceneState::RENDER_UPLOADING, ESceneState::SCENE_UPDATING))
		{
			mScene->GetCamera().Pitch(dy);
			mScene->GetCamera().RotateY(dx);
			mScene->State.has_and_transfer(ESceneState::SCENE_UPDATING, ESceneState::DIRTY, ESceneState::SCENE_UPDATING);
		}
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void TestWindow::OnMouseUp(YD3D::EMouseKey btnState, uint32_t x, uint32_t y)
{
	::ReleaseCapture();
}

void TestWindow::OnMouseDown(YD3D::EMouseKey btnState, uint32_t x, uint32_t y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	::SetCapture(GetHandle());
}

//void TestWindow::OnKeyUp(YD3D::VirtualKey key, YD3D::VirtualKeyState keyState)
//{
//	uint64_t curTick = mTimer.Tick();
//
//	if (mScene->State.null_and_add(ESceneState::UPLOADING, ESceneState::UPDATING))
//	{
//		switch (key)
//		{
//		case 'W':
//			mScene->GetCamera().Walk(0.1);
//			break;
//		case 'S':
//			mScene->GetCamera().Walk(-0.1);
//			break;
//		case 'A':
//			mScene->GetCamera().Strafe(-0.1);
//			break;
//		case 'D':
//			mScene->GetCamera().Strafe(0.1);
//			break;
//		}
//
//		mScene->State.has_and_add(ESceneState::UPDATING, ESceneState::DIRTY);
//	}
//}

//LRESULT TestWindow::MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	return DefWindowProc(GetHandle(), msg, wParam, lParam);
//}

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

	model->Create(_D3D12DEVICE_, vecVertex, vecIndex);
	mScene->Create(_D3D12DEVICE_);
	mScene->AddModel(model.get_raw_ptr());
	mScene->UpdateGraphicResource(true);
}

void TestWindow::InitResource()
{
	mDs.assign(new GraphicDepthStencil);
	mDs->Create(_D3D12DEVICE_, GetWidth(), GetHeight(), DXGI_FORMAT_D24_UNORM_S8_UINT);
	for (uint32_t i = 0; i < 2; i++) 
	{
		mRtHandle[i] = DescriptorHeapManager::GobalDescriptorHeapManager()->BindRtView(ANY_DESCRIPTOR_HEAP_POS, mSwapChain.GetBackBuffer(i), nullptr);
	}
	mDsHandle = DescriptorHeapManager::GobalDescriptorHeapManager()->BindDsView(ANY_DESCRIPTOR_HEAP_POS, mDs.get_raw_ptr(), nullptr);

	mPackage.assign(new TestResourcePackage);
	mPackage->DS = mDs;
	mPackage->DsHandle = mDsHandle;
	mPackage->State.set_state(EResourcePackageState::EINIT);
	mPackage->State.bind_callback(std::bind(&TestWindow::ResourcePackageCallback, this, std::placeholders::_1, std::placeholders::_2));

	mPipeLine.SetScene(mScene.get_raw_ptr());
	mPipeLine.PostResourcePackage(mPackage.get_raw_ptr());
}

void TestWindow::ResourcePackageCallback(YD3D::EResourcePackageState beforeState, YD3D::EResourcePackageState afterState)
{
	if (afterState == YD3D::EResourcePackageState::ERENDERING) 
	{
		uint64_t rtIndex = mSwapChain.GetCurBackBufferIndex();
		mPackage->RT = get_gc_ptr_from_raw(mSwapChain.GetCurBackBuffer());
		mPackage->RtHandle = mRtHandle[rtIndex];
		mPackage->DepthItem.RT = mPackage->RT;
		mPackage->DepthItem.RtHandle = mPackage->RtHandle;
		Update();
	}
	else if (afterState == YD3D::EResourcePackageState::ERENDERED) 
	{
		mSwapChain.Present(0, 0);
		mPipeLine.PostResourcePackage(mPackage.get_raw_ptr());
	}
}

void TestWindow::Update()
{
	if (mScene->State.has_state_weak(ESceneState::DIRTY))
	{
		mScene->State.add_state(ESceneState::RENDER_UPLOADING);
	}

	if (mScene->State.null_and_add(ESceneState::SCENE_UPDATING, ESceneState::RENDER_UPLOADING))
	{
		const float dt = mTimer.DeltaTime() * -0.01;

		if (GetAsyncKeyState('W') & 0x8000)
			mScene->GetCamera().Walk(1.0f * dt);

		if (GetAsyncKeyState('S') & 0x8000)
			mScene->GetCamera().Walk(-1.0f * dt);

		if (GetAsyncKeyState('A') & 0x8000)
			mScene->GetCamera().Strafe(-1.0f * dt);

		if (GetAsyncKeyState('D') & 0x8000)
			mScene->GetCamera().Strafe(1.0f * dt);

		mScene->UpdateSceneInfo();

		mScene->State.remove_state(ESceneState::DIRTY);
		mScene->State.remove_state(ESceneState::RENDER_UPLOADING);
	}
}



