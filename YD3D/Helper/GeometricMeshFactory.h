#pragma once
#include "YD3D_Header.h"
#include "GeometricPrimitive.h"

namespace YD3D
{
	struct MeshData 
	{
		std::vector<Vertex>		Vertices;
		std::vector<uint32_t>	Indices;
	};

	class GeometricMeshFactory 
	{
	public:
		GeometricMeshFactory();
		~GeometricMeshFactory();

		static MeshData CreateSphere(float diameter = 1, size_t tessellation = 16, bool rhcoords = true, bool invertn = false);
		static MeshData CreateBox(const DirectX::XMFLOAT3& size, bool rhcoords = true, bool invertn = false);
	};

};
