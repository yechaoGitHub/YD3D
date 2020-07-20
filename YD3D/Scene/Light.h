#include "YD3D_Header.h"

namespace YD3D
{
	struct LightDataStruct 
	{
		DirectX::XMFLOAT3 Strength = {};
		float FalloffStart = 1.0f;
		DirectX::XMFLOAT3 Diretion = {};
		float FallofEnd = 10.0f;
		DirectX::XMFLOAT3 Position = {};
		float SpotPower = 64.0f;
	};

};
