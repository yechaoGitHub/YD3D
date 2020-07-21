#include "YD3D_Header.h"

namespace YD3D
{
	struct LightDataStruct 
	{
		ALIGN_16	DirectX::XMFLOAT3 Color = {};
		float		Intensity = 0.0f;
		ALIGN_16	DirectX::XMFLOAT3 Position = {};
		float		Radius = 0.0f;
		ALIGN_16	DirectX::XMFLOAT3 Diretion = {};
	};
};
