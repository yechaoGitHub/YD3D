#pragma once
#include "GraphicResource.h"

namespace YD3D
{
	class GraphicTexture : public GraphicResource
	{
	public:
		GraphicTexture();
		~GraphicTexture();

		bool Create(ID3D12Device* device, uint32_t width, uint32_t height, uint32_t arraySize = 1, uint32_t mipLevel = 1, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
		void Release() override;

	private:
		uint32_t		mHeight;
		uint32_t		mWidth;
		DXGI_FORMAT		mFormat;
		uint32_t		mArraySize; 
		uint32_t		mMipLevel;

		uint32_t		GetWidth();
		uint32_t		GetHeight();
		DXGI_FORMAT		GetFormat();
		uint32_t		GetArraySize();
		uint32_t		GetMipLevel();
	};
};

