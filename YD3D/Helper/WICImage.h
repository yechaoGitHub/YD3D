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

		bool LoadFromMemory(uint32_t width, uint32_t height, uint32_t rowPitch, const uint8_t *data);
		bool OpenImageFile(const std::wstring& filePath);
		bool SaveFile(const std::wstring &filePath);
		void Release();

		uint32_t		Width() const;
		uint32_t		Height() const;
		uint32_t		Size() const;
		const uint8_t*	Data() const;
		
	private:
		static Microsoft::WRL::ComPtr<IWICImagingFactory>	_WIC_IMAGE_FACTORY_;
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
