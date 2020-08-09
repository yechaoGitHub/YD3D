#include "YD3D_Header.h"
#include "Resource/GraphicResourceHeader.h"
#include "PipeLine/PassTemplate.hpp"


struct DepthPassInitParam : public YD3D::PassInitParam
{
	DepthPassInitParam() 
	{
		PSODesc.RTVFormats[0] = { DXGI_FORMAT_R32_FLOAT };
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

