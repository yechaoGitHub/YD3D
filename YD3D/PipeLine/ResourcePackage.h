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
		gc_ptr<GraphicDepthStencil>		DS;
		ystate<EResourcePackageState>	State;

		void SetCallback(GraphicTaskCallbackFunction&& callback);
		bool HasCallback();
		GraphicTaskCallbackFunction&& MoveCallback();

		bool							mHasCallback;
		GraphicTaskCallbackFunction		mCallback;

		void ResourcePackageCallBack(D3D12_COMMAND_LIST_TYPE type, uint64_t fence);
	
	};

}
