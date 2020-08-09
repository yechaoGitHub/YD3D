#pragma once
#include "YD3D_Header.h"
#include "PipeLine/PassTemplate.hpp"
#include "PipeLine/ResourcePackage.h"
#include "Scene/Camera.h"

namespace YD3D
{
	struct LatLongToCubeMapInitParam : public PassInitParam
	{
		LatLongToCubeMapInitParam() 
		{
			PSODesc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		}
	};

	struct LatLongToCubeMapRenderItem 
	{
		GraphicTexture				*LatLongTexture;
		GraphicRenderTarget			*RenderTarget;
	};

	enum ECubeCameraDirection { ERIGHT, ELEFT, ETOP, EBOTTOM, EBACK, EFRONT };

	class LatLongToCubeMapPass : public PassTemplate<LatLongToCubeMapRenderItem, LatLongToCubeMapInitParam>
	{
		typedef PassTemplate<LatLongToCubeMapRenderItem, LatLongToCubeMapInitParam> Super;

	public:
		LatLongToCubeMapPass();
		~LatLongToCubeMapPass();

		bool Create(ID3D12Device* device, const LatLongToCubeMapInitParam* initParam = nullptr) override;
		void SetViewProjIndex(ECubeCameraDirection direction);
		void SetSize(uint32_t width, uint32_t height);
		void InitViewProjMatrix();
		bool PopulateCommandList(LatLongToCubeMapRenderItem* package, ID3D12GraphicsCommandList* commandList) override;
		bool PopulateCommandList(GraphicTexture* latLongTexture, GraphicRenderTarget* renderTarget, ID3D12GraphicsCommandList* commandList);

	private:
		Camera												mCamera;
		GraphicConstBuffer<DirectX::XMFLOAT4X4>				mCbBuffer;
		uint32_t											mCurMatrixIndex;
		GraphicVertexBuffer									mVertexBuffer;
		GraphicIndexBuffer									mIndexBuffer;
		GraphicUploadBuffer									mUploadBuffer;
		uint32_t											mPreIndexCount;
		uint32_t											mWidth;
		uint32_t											mHeight;
		bool SerializeRootSignature() override;
	};
};
