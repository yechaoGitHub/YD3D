#include "TestWindow.h"
#include "Helper/GeometricMeshFactory.h"
#include "Util/CommonVertexIndexBuffer.h"
#include "Helper/LatLongToCubeMap.h"
#include "Helper/UploadHelper.h"

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
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

void TestWindow::OnMouseMove(YD3D::EMouseKey btnState, uint32_t x, uint32_t y)
{
	if ((static_cast<uint64_t>(btnState) & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>((int32_t)x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>((int32_t)y - mLastMousePos.y));

		Scene *pScene = mScene.get_raw_ptr();
		mScene->StateBarrier(
			[pScene](ystate<ESceneState>& state)
			{
				return state.null_and_add(ESceneState::RENDER_UPLOADING, ESceneState::SCENE_UPDATING);
			},
			[pScene, dx, dy](ystate<ESceneState>& state)
			{
				pScene->Pitch(dy);
				pScene->RotateY(dx);
				state.remove_state(ESceneState::SCENE_UPDATING);
			});
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

	CommonVertexIndexBuffer::Initialize(mDxFactory.Device());
	UploadHelper::Initialize(mDxFactory.Device());
}

void TestWindow::InitScence()
{	
	MeshData &&meshData = GeometricMeshFactory::CreateSphere(1, 64, false);

	mImages[EBASE_COLOR].OpenImageFile(L"DemoResource/rustediron2_basecolor.png");
	mImages[EMETALLIC].OpenImageFile(L"DemoResource/rustediron2_metallic.png");
	mImages[ENORMAL].OpenImageFile(L"DemoResource/rustediron2_normal.png");
	mImages[EROUGHNESS].OpenImageFile(L"DemoResource/rustediron2_roughness.png");
	mLatlongImage.OpenImageFile(L"DemoResource/Ice_Lake_HiRes_TMap.jpg");
	mLatLongResource.Create(mDxFactory.Device(), mLatlongImage.Width(), mLatlongImage.Height());

	UploadHelper::UploadTexture(&mLatLongResource, 0, mLatlongImage.Data(), mLatlongImage.Size());

	DescriptorHeapManager::GobalDescriptorHeapManager()->BindSrView(ANY_DESCRIPTOR_HEAP_POS,  &mLatLongResource, nullptr);

	gc_ptr<Model> model;
	model.assign(new Model);
	mScene.assign(new Scene);

	model->Create(_D3D12DEVICE_, meshData.Vertices, meshData.Indices);
	mScene->Create(_D3D12DEVICE_);
	mScene->AddModel(model.get_raw_ptr());
	mScene->AddPointLight(XMFLOAT3(1, 1, 1), 500, XMFLOAT3(0, 5, -5), 10);
	mScene->AddSpotLight(3, XMFLOAT3(4, 5, 6), XMFLOAT3(0, 1, 0));

	model->UpdateTexture(mImages, 4);

	uint64_t fenceValue = GraphicTask::PostGraphicTask(ECommandQueueType::ECOPY, [model](ID3D12GraphicsCommandList* commandList) mutable {model->UpdateGraphicResource(commandList); });
	GraphicTask::WaitForGraphicTaskCompletion(ECommandQueueType::ECOPY, fenceValue);

	GraphicResource* arrGpuRes[] = {
		model->GraphicResource().mTextures[0].get_raw_ptr(),
		model->GraphicResource().mTextures[1].get_raw_ptr(),
		model->GraphicResource().mTextures[2].get_raw_ptr(),
		model->GraphicResource().mTextures[3].get_raw_ptr(),
	};

	DescriptorHeapManager::GobalDescriptorHeapManager()->BindSrView(ANY_DESCRIPTOR_HEAP_POS, 4, arrGpuRes, nullptr);

	mScene->UpdateLightParam();
	mScene->UpdateDrawParam();
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
	mPackage->DsHandle = mDs->GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 0);
	mPackage->State.set_state(EResourcePackageState::EINIT);
	mPackage->State.bind_callback(std::bind(&TestWindow::ResourcePackageCallback, this, std::placeholders::_1, std::placeholders::_2));

	mPackage->LatlongItem.LatLongTexture = &mLatLongResource;
	mPackage->LatlongItem.DepthStencil = mDs.get_raw_ptr();

	mPipeLine.SetScene(mScene.get_raw_ptr());
	mPipeLine.PostResourcePackage(mPackage.get_raw_ptr());
}

void TestWindow::ResourcePackageCallback(YD3D::EResourcePackageState beforeState, YD3D::EResourcePackageState afterState)
{
	if (afterState == YD3D::EResourcePackageState::ERENDERING) 
	{
		gc_ptr<GraphicRenderTarget> gcRt = get_gc_ptr_from_raw(mSwapChain.GetCurBackBuffer());
		D3D12_CPU_DESCRIPTOR_HANDLE rtHandle = gcRt->GetCpuDescriptorHandle(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 0);
		uint64_t rtIndex = mSwapChain.GetCurBackBufferIndex();
		mPackage->RT = gcRt;
		mPackage->RtHandle = rtHandle;
		mPackage->DepthItem.RT = gcRt;
		mPackage->DepthItem.RtHandle = rtHandle;
		mPackage->LatlongItem.RenderTarget = gcRt.get_raw_ptr();
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
	if (mScene->State().has_state_weak(static_cast<ESceneState>(CAMERA_DIRTY|LIGHT_PARAM_DIRTY|LIGHT_DIRTY|DRAW_PARAM_DIRTY|VERTEX_INDEX_DIRTY)))
	{
		mScene->State().add_state(ESceneState::RENDER_UPLOADING);
	}

	if (mScene->State().null_and_add(ESceneState::SCENE_UPDATING, ESceneState::RENDER_UPLOADING))
	{
		const float dt = mTimer.DeltaTime() * -0.01;

		if (GetAsyncKeyState('W') & 0x8000)
			mScene->Walk(1.0f * dt);

		if (GetAsyncKeyState('S') & 0x8000)
			mScene->Walk(-1.0f * dt);

		if (GetAsyncKeyState('A') & 0x8000)
			mScene->Strafe(-1.0f * dt);

		if (GetAsyncKeyState('D') & 0x8000)
			mScene->Strafe(1.0f * dt);

		mScene->UpdateGraphicResource(true);

		mScene->State().remove_state(ESceneState::RENDER_UPLOADING);
	}
}



