#include "GraphicTexture.h"

namespace YD3D
{
	GraphicTexture::GraphicTexture():
		mHeight(0),
		mWidth(0),
		mFormat(DXGI_FORMAT_UNKNOWN),
		mArraySize(0),
		mMipLevel(0)
	{

	}

	GraphicTexture::~GraphicTexture()
	{

	}

	bool GraphicTexture::Create(ID3D12Device* device, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevel, DXGI_FORMAT format)
	{
		bool b = GraphicResource::Create(
			device,
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, arraySize, mipLevel, 1, 0, D3D12_RESOURCE_FLAG_NONE),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr);

		if (b) 
		{
			mHeight = height;
			mWidth = width;
			mFormat = format;
			mArraySize = arraySize;
			mMipLevel = mipLevel;
		}

		return b;
	}

	void GraphicTexture::Release()
	{
		GraphicResource::Release();
		mHeight = 0;
		mWidth = 0;
		mFormat = DXGI_FORMAT_UNKNOWN;
		mArraySize = 0;
		mMipLevel = 0;
	}

	uint32_t GraphicTexture::GetWidth()
	{
		return mWidth;
	}

	uint32_t GraphicTexture::GetHeight()
	{
		return mHeight;
	}

	DXGI_FORMAT GraphicTexture::GetFormat()
	{
		return mFormat;
	}

	uint32_t GraphicTexture::GetArraySize()
	{
		return mArraySize;
	}

	uint32_t GraphicTexture::GetMipLevel()
	{
		return mMipLevel;
	}
};
