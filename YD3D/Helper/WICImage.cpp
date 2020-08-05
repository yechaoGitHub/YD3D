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

	bool WICImage::LoadFromRaw(uint32_t width, uint32_t height, uint32_t rowPitch, const uint8_t * data)
	{
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateBitmapFromMemory(
			width, 
			height, 
			GUID_WICPixelFormat32bppRGBA, 
			rowPitch, 
			rowPitch * height, 
			const_cast<uint8_t*>(data), 
			&mBitmap));


		return true;
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

		Microsoft::WRL::ComPtr<IWICFormatConverter> pIConverter;
		Microsoft::WRL::ComPtr<IWICBitmapSource> bitmapSource;
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateFormatConverter(&pIConverter));
		ThrowIfFailed(pIConverter->Initialize(
			mFrame.Get(),               
			GUID_WICPixelFormat32bppRGBA,						 
			WICBitmapDitherTypeNone,        
			NULL,                            
			0.f,                            
			WICBitmapPaletteTypeCustom));

		ThrowIfFailed(pIConverter.As(&bitmapSource));

		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateBitmapFromSource(
			bitmapSource.Get(),
			WICBitmapCacheOnDemand,
			&mBitmap));
	
		ThrowIfFailed(mFrame->GetSize(&mWidth, &mHeight));

		WICRect rt = { 0, 0, mWidth, mHeight};
		ThrowIfFailed(mBitmap->Lock(&rt, WICBitmapLockRead, &mBitmapLock));
		ThrowIfFailed(mBitmapLock->GetDataPointer(&mByteSize, &mData));

		mFilePath = filePath;

		return true;
	}

	bool WICImage::SaveFile(const std::string &filePath)
	{
		Microsoft::WRL::ComPtr<IWICBitmapEncoder> spBitmapEncoder;
		Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> spFrameEncode;

	

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

	uint32_t WICImage::Width() const
	{
		return mWidth;
	}

	uint32_t WICImage::Height() const
	{
		return mHeight;
	}

	uint32_t WICImage::Size() const
	{
		return mByteSize;
	}

	const uint8_t* WICImage::Data() const
	{
		return mData;
	}
};
