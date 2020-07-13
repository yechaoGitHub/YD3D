#pragma once
#include "YD3D_Header.h"
#include "Scene/Scene.h"

namespace YD3D
{
	struct PassInitParam
	{
		std::vector<D3D12_INPUT_ELEMENT_DESC>	InputLayout;
		std::vector<DXGI_FORMAT>				RenderTargetFormat;
		DXGI_FORMAT								DepthStencilFormat;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE			PrimitiveType;
	};

	template<typename TResourcePackage, typename TPassInitParam = PassInitParam>
	class PassTemplate
	{
	public:
		PassTemplate()
		{
		}

		virtual ~PassTemplate()
		{
		}

		virtual bool Create(ID3D12Device* device, const TPassInitParam *initParam)
		{
			return false;
		}

		virtual bool BindToRootSignature(TResourcePackage* package)
		{
			return false;
		}

		virtual bool PopulateCommandList(TResourcePackage* package, ID3D12GraphicsCommandList* commandList)
		{
			return false;
		}

		virtual bool PostToCommandQueue(TResourcePackage* package, ECommandQueueType type)
		{
			return false;
		}

		virtual bool WaitForCompletion() 
		{
			return false;
		}

		virtual bool SetScene(Scene* scene)
		{
			mScene = scene;
			return true;
		}

	protected:
		ID3D12Device*										mDevice;
		TPassInitParam										mInitParam;
		Microsoft::WRL::ComPtr<ID3D12PipelineState>			mPSO;
		Microsoft::WRL::ComPtr<ID3D12RootSignature>			mRootSignature;
		Microsoft::WRL::ComPtr<ID3D10Blob>					mSerializedRootSignature;
		std::array<Microsoft::WRL::ComPtr<ID3DBlob>, 5>		mArrShaderRes;
		uint64_t											mFenceValue;
		ECommandQueueType									mCommandQueueType;
		Scene*												mScene;
	};

};
