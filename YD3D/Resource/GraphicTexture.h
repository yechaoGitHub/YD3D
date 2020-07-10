#pragma once
#include "GraphicResource.h"

namespace YD3D
{
	class GraphicTexture : public GraphicResource
	{
	public:
		GraphicTexture();
		~GraphicTexture();

		bool Create(ID3D12Device* device, UINT width, UINT height, UINT arraySize = 1, UINT mipLevel = 1, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	};
};

