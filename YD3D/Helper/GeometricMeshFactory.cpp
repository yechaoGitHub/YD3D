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

		ConvertToYD3DMeshData(vertices, indices, meshData);

		return std::move(meshData);
	}

	MeshData GeometricMeshFactory::CreateBox(const DirectX::XMFLOAT3& size, bool rhcoords, bool invertn)
	{
		MeshData meshData;
		std::vector<GeometricPrimitive::VertexType> vertices;
		std::vector<uint16_t> indices;

		GeometricPrimitive::CreateBox(vertices, indices, size, rhcoords, invertn);

		ConvertToYD3DMeshData(vertices, indices, meshData);

		return std::move(meshData);
	}

	void GeometricMeshFactory::ConvertToYD3DMeshData(const std::vector<DirectX::GeometricPrimitive::VertexType>& vertices, const std::vector<uint16_t>& indices, MeshData& meshData)
	{
		meshData.Vertices.assign(vertices.size(), YD3D::Vertex());
		meshData.Indices.assign(indices.size(), 0);

		for (uint32_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t in1 = indices[i];
			uint32_t in2 = indices[i + 1];
			uint32_t in3 = indices[i + 2];

			const GeometricPrimitive::VertexType& v0 = vertices[in1];
			const GeometricPrimitive::VertexType& v1 = vertices[in2];
			const GeometricPrimitive::VertexType& v2 = vertices[in3];

			DirectX::XMFLOAT3 edge1(v1.position.x - v0.position.x, v1.position.y - v0.position.y, v1.position.z - v0.position.z);
			DirectX::XMFLOAT3 edge2(v2.position.x - v0.position.x, v2.position.y - v0.position.y, v2.position.z - v0.position.z);

			float deltaU1 = v1.textureCoordinate.x - v0.textureCoordinate.x;
			float deltaV1 = v1.textureCoordinate.y - v0.textureCoordinate.y;
			float deltaU2 = v2.textureCoordinate.x - v0.textureCoordinate.x;
			float deltaV2 = v2.textureCoordinate.y - v0.textureCoordinate.y;

			float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

			XMFLOAT3 tangent = {};

			tangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
			tangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
			tangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

			meshData.Vertices[in1] = YD3D::Vertex(v0.position, v0.normal, tangent, v0.textureCoordinate);
			meshData.Vertices[in2] = YD3D::Vertex(v1.position, v1.normal, tangent, v1.textureCoordinate);
			meshData.Vertices[in3] = YD3D::Vertex(v2.position, v2.normal, tangent, v2.textureCoordinate);

			meshData.Indices[i] = in1;
			meshData.Indices[i + 1] = in2;
			meshData.Indices[i + 2] = in3;
		}
	}
};
