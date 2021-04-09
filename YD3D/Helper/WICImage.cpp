#include "WICImage.h"
#pragma once

namespace YD3D
{
	using namespace Microsoft::WRL;

	ComPtr<IWICImagingFactory> WICImage::_WIC_IMAGE_FACTORY_(nullptr);

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

	bool WICImage::LoadFromMemory(uint32_t width, uint32_t height, uint32_t rowPitch, const uint8_t *data)
	{
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateBitmapFromMemory(
			width, 
			height, 
			GUID_WICPixelFormat32bppRGBA, 
			rowPitch, 
			rowPitch * height, 
			const_cast<uint8_t*>(data), 
			&mBitmap));

		mWidth = width;
		mHeight = height;
		mByteSize = rowPitch * mHeight;
		mPixelFormat = GUID_WICPixelFormat32bppRGBA;

		ThrowIfFailed(mBitmap->Lock(nullptr, WICBitmapLockRead, &mBitmapLock));
		ThrowIfFailed(mBitmapLock->GetDataPointer(&mByteSize, &mData));

		return true;
	}

	bool WICImage::OpenImageFile(const std::wstring& filePath)
	{
		ComPtr<IWICBitmapDecoder>		decoder;
		ComPtr<IWICBitmapFrameDecode>	frame;
		ComPtr<IWICFormatConverter>						converter;
		ComPtr<IWICBitmapSource>						bitmapSource;

		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateDecoderFromFilename(
			filePath.c_str(),
			nullptr,
			GENERIC_READ, 
			WICDecodeMetadataCacheOnDemand, 
			&decoder));

		ThrowIfFailed(decoder->GetFrame(0, &frame));


		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateFormatConverter(&converter));
		ThrowIfFailed(converter->Initialize(
			frame.Get(),               
			GUID_WICPixelFormat32bppRGBA,						 
			WICBitmapDitherTypeNone,        
			NULL,                            
			0.f,                            
			WICBitmapPaletteTypeCustom));

		ThrowIfFailed(converter.As(&bitmapSource));
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateBitmapFromSource(
			bitmapSource.Get(),
			WICBitmapCacheOnDemand,
			&mBitmap));
	
		ThrowIfFailed(mBitmap->GetSize(&mWidth, &mHeight));
		ThrowIfFailed(mBitmap->GetPixelFormat(&mPixelFormat));
		ThrowIfFailed(mBitmap->Lock(nullptr, WICBitmapLockRead, &mBitmapLock));
		ThrowIfFailed(mBitmapLock->GetDataPointer(&mByteSize, &mData));

		mFilePath = filePath;

		return true;
	}

	bool WICImage::SaveFile(const std::wstring &filePath)
	{
		ComPtr<IWICStream>					stream;
		ComPtr<IWICBitmapEncoder>			bitmapEncoder;
		ComPtr<IWICBitmapFrameEncode>		frameEncode;
		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateStream(&stream));
		ThrowIfFailed(stream->InitializeFromFilename(filePath.c_str(), GENERIC_WRITE));

		ThrowIfFailed(_WIC_IMAGE_FACTORY_->CreateEncoder(GUID_ContainerFormatPng, nullptr, &bitmapEncoder));
		ThrowIfFailed(bitmapEncoder->Initialize(stream.Get(), WICBitmapEncoderNoCache));

		ThrowIfFailed(bitmapEncoder->CreateNewFrame(&frameEncode, nullptr));
		ThrowIfFailed(frameEncode->Initialize(nullptr));

		ThrowIfFailed(frameEncode->SetSize(mWidth, mHeight));
		WICPixelFormatGUID format = GUID_WICPixelFormatDontCare;
		ThrowIfFailed(frameEncode->SetPixelFormat(&format));

		ThrowIfFailed(frameEncode->WriteSource(mBitmap.Get(), nullptr));
		ThrowIfFailed(frameEncode->Commit());
		ThrowIfFailed(bitmapEncoder->Commit());

		return true;
	}

	void WICImage::Release()
	{
		mBitmapLock = nullptr;
		mBitmap = nullptr;
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
