#include "YD3D_Header.h"
#include "Resource/GraphicResourceHeader.h"
#include "PipeLine/PassTemplate.hpp"


struct DepthPassInitParam : public YD3D::PassInitParam
{
	DepthPassInitParam() 
	{
		InputLayout = YD3D::GetCommonInputLayout();
		RenderTargetFormat = { DXGI_FORMAT_R32_FLOAT };
		DepthStencilFormat = { DXGI_FORMAT_D24_UNORM_S8_UINT };
		PrimitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
};

struct DepthPassRenderItem
{
	gc_ptr<YD3D::GraphicRenderTarget>	RT;
	D3D12_CPU_DESCRIPTOR_HANDLE			RtHandle;
};

class DepthPass : public YD3D::PassTemplate<DepthPassRenderItem, DepthPassInitParam>
{
public:
	DepthPass();
	~DepthPass();

	bool PopulateCommandList(DepthPassRenderItem* renderItem, ID3D12GraphicsCommandList* commandList);

protected:
	bool SerializeRootSignature() override;
};

