#pragma once
#include "YD3D_Header.h"
#include "wincodec.h"

namespace YD3D
{
	class WICImage 
	{
	public:
		WICImage();
		~WICImage();

		static void Initialize();

		bool OpenImageFile(const std::wstring& filePath);
		void Release();

		uint32_t		Width();
		uint32_t		Height();
		uint32_t		Size();
		uint8_t*		Data();
		
	private:
		static Microsoft::WRL::ComPtr<IWICImagingFactory>	_WIC_IMAGE_FACTORY_;
		Microsoft::WRL::ComPtr<IWICBitmapDecoder>			mDecoder;
		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode>		mFrame;
		Microsoft::WRL::ComPtr<IWICBitmap>					mBitmap;
		Microsoft::WRL::ComPtr<IWICBitmapLock>				mBitmapLock;
		WICPixelFormatGUID									mPixelFormat;
		uint32_t											mWidth;
		uint32_t											mHeight;
		uint32_t											mByteSize;
		uint8_t*											mData;
		std::wstring	mFilePath;
	};

};
