#pragma once
#include "YD3D_Header.h"
#include "GraphicResource.h"

namespace YD3D
{
	class GraphicDepthStencil : public GraphicResource
	{
	public:
		GraphicDepthStencil();
		~GraphicDepthStencil();

		bool Create(ID3D12Device* device, UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
	};

};
