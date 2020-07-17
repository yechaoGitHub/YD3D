#pragma once
#include "Window/YWindow.h"
#include "YD3D_Header.h"
#include "Graph/DxFactory.h"
#include "Graph/GraphicTask.h"
#include "Graph/DescriptorHeapManager.h"
#include "Graph/SwapChain.h"
#include "Scene/Scene.h"
#include "Scene/Model.h"
#include "TestPipeLine.h"
#include "Helper/WICImage.h"

class TestWindow : public YD3D::YWindow
{
	enum EImage { EBASE_COLOR, EMETALLIC, ENORMAL, EROUGHNESS };

public:
	TestWindow();
	virtual ~TestWindow();
	static ID3D12Device*		_D3D12DEVICE_;

	bool Create(uint32_t width, uint32_t height, const std::wstring& caption = L"") override;
	uint32_t Run() override;

protected:
//	LRESULT MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

	void OnMouseMove(YD3D::EMouseKey btnState, uint32_t x, uint32_t y) override;
	void OnMouseUp(YD3D::EMouseKey btnState, uint32_t x, uint32_t y) override;
	void OnMouseDown(YD3D::EMouseKey btnState, uint32_t x, uint32_t y) override;
	//void OnKeyUp(YD3D::VirtualKey key, YD3D::VirtualKeyState keyState) override;

private:
	YD3D::DxFactory					mDxFactory;
	YD3D::GraphicTask				mGraphTask;
	YD3D::DescriptorHeapManager		mDescriptorManager;
	YD3D::SwapChain					mSwapChain;
	YD3D::GameTimer					mTimer;
	uint64_t						mPreTick;
	uint64_t						mPreFPSTick;
	uint64_t						mSecondTick;
	uint64_t						mPreSecondTick;

	D3D12_CPU_DESCRIPTOR_HANDLE		mRtHandle[2];
	D3D12_CPU_DESCRIPTOR_HANDLE		mDsHandle;

	TestPipeLine						mPipeLine;
	gc_ptr<YD3D::Scene>					mScene;
	gc_ptr<TestResourcePackage>			mPackage;
	gc_ptr<YD3D::GraphicDepthStencil>	mDs;
	POINT								mLastMousePos;
	YD3D::WICImage						mImages[4];
	

	void InitD3D();
	void InitScence();
	void InitResource();
	void CalculateFrameStats();
	void ResourcePackageCallback(YD3D::EResourcePackageState beforeState, YD3D::EResourcePackageState afterState);
	void Update();
};
