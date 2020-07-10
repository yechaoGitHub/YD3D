#pragma once
#include "GraphicResource.h"

namespace YD3D
{
	class GraphicReadbackBuffer : public GraphicResource
	{
	public:
		GraphicReadbackBuffer();
		~GraphicReadbackBuffer();

		bool Create(ID3D12Device* device, uint64_t length);

	};
};


