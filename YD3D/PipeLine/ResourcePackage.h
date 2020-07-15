#pragma once
#include "YD3D_Header.h"
#include "Resource/GraphicResourceHeader.h"

namespace YD3D 
{
	enum EResourcePackageState { EINIT = 0, EPOSTED, ERENDERING, ERENDERED };

	struct ResourcePackage : public virtual enable_gc_ptr_form_raw
	{
		enum { need_clear_up_gc_ptr = 0 };

		ResourcePackage();
		virtual ~ResourcePackage();

		uint64_t						TimeStamp;
		gc_ptr<GraphicRenderTarget>		RT;
		D3D12_CPU_DESCRIPTOR_HANDLE		RtHandle;
		gc_ptr<GraphicDepthStencil>		DS;
		D3D12_CPU_DESCRIPTOR_HANDLE		DsHandle;
		ystate<EResourcePackageState>	State;
	};

}
