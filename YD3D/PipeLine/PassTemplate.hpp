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

		virtual bool Create(ID3D12Device* device, const TPassInitParam *initParam = nullptr)
		{
			mDevice = device;
			if (initParam != nullptr) 
			{
				mInitParam = *initParam;
			}
			
			SerializeRootSignature();
			BuildRootSignature();
			BuildShaderResource();
			BuildPSO();

			return true;
		}

		virtual bool BindToRootSignature(TResourcePackage* package)
		{
			return true;
		}

		virtual bool PopulateCommandList(TResourcePackage* package, ID3D12GraphicsCommandList* commandList)
		{
			BindToRootSignature(package);

			for (auto& pair : mScene->GetDrawParam())
			{
				const DrawParam& drawParam = pair.second;
				Model* model = drawParam.Model;
				commandList->DrawIndexedInstanced(drawParam.IndexCountPerInstance, 1, drawParam.StartIndexLocation, drawParam.BaseVertexLocation, drawParam.StartInstanceLocation);
			}
			return true;
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
		std::array<std::wstring, 5>							mArrShaderResPath;
		uint64_t											mFenceValue;
		ECommandQueueType									mCommandQueueType;
		Scene*												mScene;

		virtual bool SerializeRootSignature()
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(
				0,
				nullptr,
				0,
				nullptr,
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
			ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &mSerializedRootSignature, &errorBlob));

			if (errorBlob != nullptr)
			{
				::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			return true;
		}
		
		virtual bool BuildRootSignature()
		{
			ThrowIfFailed(mDevice->CreateRootSignature(0, mSerializedRootSignature->GetBufferPointer(), mSerializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)));
			return true;
		}

		virtual bool BuildShaderResource() 
		{
			for (uint32_t i = 0; i < 5; i++) 
			{
				std::wstring shaderPath = mArrShaderResPath[i];

				if (!shaderPath.empty())
				{
					mArrShaderRes[i] = CompileShader(shaderPath.c_str(), nullptr, "main", GetDefaultShaderModel(static_cast<EShaderType>(i)));
				}
			}

			return true;
		}

		virtual bool BuildPSO()
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
			desc.InputLayout = { mInitParam.InputLayout.data(), (UINT)mInitParam.InputLayout.size() };
			desc.pRootSignature = mRootSignature.Get();
			desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			desc.SampleMask = UINT_MAX;
			desc.PrimitiveTopologyType = mInitParam.PrimitiveType;
			desc.NumRenderTargets = mInitParam.RenderTargetFormat.size();
			desc.RTVFormats[0] = mInitParam.RenderTargetFormat[0];
			desc.DSVFormat = mInitParam.DepthStencilFormat;
			desc.SampleDesc.Count = 1;

			if (!mArrShaderResPath[EShaderType::VS].empty())
			{
				desc.VS = { mArrShaderRes[EShaderType::VS]->GetBufferPointer(), mArrShaderRes[EShaderType::VS]->GetBufferSize() };
			}

			if (!mArrShaderResPath[EShaderType::PS].empty())
			{
				desc.PS = { mArrShaderRes[EShaderType::PS]->GetBufferPointer(), mArrShaderRes[EShaderType::PS]->GetBufferSize() };
			}

			if (!mArrShaderResPath[EShaderType::DS].empty())
			{
				desc.DS = { mArrShaderRes[EShaderType::DS]->GetBufferPointer(), mArrShaderRes[EShaderType::DS]->GetBufferSize() };
			}

			if (!mArrShaderResPath[EShaderType::GS].empty())
			{
				desc.GS = { mArrShaderRes[EShaderType::GS]->GetBufferPointer(), mArrShaderRes[EShaderType::GS]->GetBufferSize() };
			}

			if (!mArrShaderResPath[EShaderType::HS].empty())
			{
				desc.HS = { mArrShaderRes[EShaderType::HS]->GetBufferPointer(), mArrShaderRes[EShaderType::HS]->GetBufferSize() };
			}

			ThrowIfFailed(mDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&mPSO)));

			return true;
		}

		virtual std::string GetDefaultShaderModel(EShaderType type) 
		{
			switch (type) 
			{
			case EShaderType::VS:
				return "vs_5_1";
			case EShaderType::PS:
				return "ps_5_1";
			case EShaderType::DS:
				return "cs_5_1";	
			case EShaderType::HS:
				return "hs_5_1";
			case EShaderType::GS:
				return "gs_5_1";

			default:
				assert(0);
			}

			return "";
		}

	};

};
