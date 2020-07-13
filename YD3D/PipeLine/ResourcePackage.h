#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicResourceHeader.h"

namespace YD3D 
{
	struct ResourcePackage : public virtual enable_gc_ptr_form_raw
	{
		enum { need_clear_up_gc_ptr = 0 };

		ResourcePackage();
		virtual ~ResourcePackage();

		gc_ptr<GraphicRenderTarget>		RT;
		D3D12_CPU_DESCRIPTOR_HANDLE		RtHandle;
		gc_ptr<GraphicDepthStencil>		DS;
		D3D12_CPU_DESCRIPTOR_HANDLE		DsHandle;
		ystate<EResourcePackageState>	State;
		
		void BindCallback(GraphicTaskCallbackFunction &&UserCallback);
		GraphicTaskCallbackFunction& UserCallback();
		GraphicTaskCallbackFunction& PackageCallback();

	private:
		GraphicTaskCallbackFunction		mPackageCallback;
		bool							mHasUserCallback;
		GraphicTaskCallbackFunction		mUserCallback; 

		void ResourcePackageCallBack(D3D12_COMMAND_LIST_TYPE type, uint64_t fence);
	};

}
