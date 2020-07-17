#include "WICImage.h"
#pragma once

namespace YD3D
{
	Microsoft::WRL::ComPtr<IWICImagingFactory> WICImage::_WIC_IMAGE_FACTORY_(nullptr);

	WICImage::WICImage() :
		mPixelFormat({}),
		mWidth(0),
		mHeight(0),
		mByteSize(0),
		mData(nullptr)
	{
	}

	WICImage::~WICImage()
	{
		Release();
	}

	void WICImage::Initialize()
	{
		ThrowIfFailed(CoInitialize(nullptr));

		ThrowIfFailed(CoCreateInstance(CLSID_WICImagingFactory, 
			nullptr, 
			CLSCTX_INPROC_SERVER, 
			IID_PPV_ARGS(&_WIC_IMAGE_FACTORY_)));
	}

	bool WICImage::OpenImageFile(const std::wstring& filePath)
	{
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateDecoderFromFilename(
			filePath.c_str(),
			nullptr,
			GENERIC_READ, 
			WICDecodeMetadataCacheOnDemand, 
			&mDecoder));

		ThrowIfFailed(mDecoder->GetFrame(0, &mFrame));

		ThrowIfFailed(mFrame->GetPixelFormat(&mPixelFormat));

		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateBitmapFromSource(
			mFrame.Get(),
			WICBitmapCacheOnDemand,
			&mBitmap));

		ThrowIfFailed(mFrame->GetSize(&mWidth, &mHeight));

		WICRect rt = { 0, 0, mWidth, mHeight};
		ThrowIfFailed(mBitmap->Lock(&rt, WICBitmapLockRead, &mBitmapLock));
		ThrowIfFailed(mBitmapLock->GetDataPointer(&mByteSize, &mData));

		mFilePath = filePath;

		return true;
	}

	void WICImage::Release()
	{
		mDecoder = nullptr;
		mFrame = nullptr;
		mBitmap = nullptr;
		mBitmapLock = nullptr;
		mWidth = 0;
		mHeight = 0;
		mByteSize = 0;
		mData = nullptr;
	}

	uint32_t WICImage::Width()
	{
		return mWidth;
	}

	uint32_t WICImage::Height()
	{
		return mHeight;
	}

	uint32_t WICImage::Size()
	{
		return mByteSize;
	}

	uint8_t* WICImage::Data()
	{
		return mData;
	}
};
