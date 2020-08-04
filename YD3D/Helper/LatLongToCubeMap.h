#include "YD3D_Header.h"
#include "Helper/WICImage.h"
#include "PipeLine/LatLongToCubeMapPass.h"
#include "Resource/GraphicTexture.h"

namespace YD3D
{
	class LatLongToCubeMap 
	{
	public:
		LatLongToCubeMap();
		~LatLongToCubeMap();
		
		bool GenerateCubeMap(ID3D12Device* device, const WICImage& latLongImage, uint32_t width, uint32_t height);

	private:
		ID3D12Device*				mDevice;
		uint32_t					mWidth;
		uint32_t					mHeight;
		GraphicUploadBuffer			mUploadBuffer;
		GraphicReadbackBuffer		mReadbackBuffer;
		GraphicTexture				mCubeMapTexture;
		GraphicTexture				mLatLongTexture;
		GraphicRenderTarget			mRendetTarget;
		LatLongToCubeMapPass		mPass;

		void SetUp(const WICImage& latLongImage, uint32_t width, uint32_t height);
		void LatLongToCubeMapTask(ID3D12GraphicsCommandList *commandList);
		void ClearUp();
	
	};
};
