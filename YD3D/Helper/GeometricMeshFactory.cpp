#include "GeometricMeshFactory.h"

using namespace DirectX;

namespace YD3D
{
	GeometricMeshFactory::GeometricMeshFactory()
	{
	}

	GeometricMeshFactory::~GeometricMeshFactory()
	{
	}

	MeshData GeometricMeshFactory::CreateSphere(float diameter, size_t tessellation, bool rhcoords, bool invertn)
	{
		MeshData meshData;
		std::vector<GeometricPrimitive::VertexType> vertices;
		std::vector<uint16_t> indices;

		GeometricPrimitive::CreateSphere(vertices, indices, diameter, tessellation, rhcoords, invertn);

		for (auto &vertex : vertices)
		{
			XMFLOAT3 tangent = {};

			meshData.Vertices.push_back(YD3D::Vertex(vertex.position, vertex.normal, tangent, vertex.textureCoordinate));
		}

		for (auto &index : indices)
		{
			meshData.Indices.push_back(index);
		}

		return std::move(meshData);
	}
};
