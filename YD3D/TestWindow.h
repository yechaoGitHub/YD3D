#pragma once
#include "Window/YWindow.h"
#include "Graph/DxFactory.h"
#include "Graph/GraphicTask.h"
#include "Scene/Scene.h"
#include "Scene/Model.h"

class TestWindow : public YD3D::YWindow
{
public:
	TestWindow();
	virtual ~TestWindow();
	static ID3D12Device*		_D3D12DEVICE_;

	bool Create(uint32_t width, uint32_t height, const std::wstring& caption = L"") override;

protected:
	LRESULT MainWndProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	YD3D::DxFactory				mDxFactory;
	YD3D::GraphicTask			mGraphTask;
	gc_ptr<YD3D::Scene>			mScene;
	
	void InitD3D();
	void BuildResource();
};
