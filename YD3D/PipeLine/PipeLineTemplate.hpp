#pragma once
#include "YD3D_Header.h"
#include "ResourcePackage.h"
#include "Graph/GraphicTask.h"
#include "Scene/Scene.h"

namespace YD3D 
{
	struct PipeLineInitParam
	{
		PipeLineInitParam() :
			outputWidth(0),
			outputHeight(0)
		{
		}

		uint32_t	outputWidth;
		uint32_t	outputHeight;
	};

	template<typename TResourcePackage, typename TPipeLineInitParam>
	class PipeLineTemplate
	{
	public:
		PipeLineTemplate() 
		{
			static_assert(std::is_base_of_v<ResourcePackage, TResourcePackage>, L"±ØÐë¼Ì³ÐResourcePackage");
			static_assert(std::is_base_of_v<PipeLineInitParam, TPipeLineInitParam>, L"±ØÐë¼Ì³ÐPipeLineInitParam");
		}

		virtual ~PipeLineTemplate()
		{

		}

		virtual bool SetScene(Scene *scene)
		{
			mScene = get_gc_ptr_from_raw(scene);
			return true;
		}

		virtual bool Create(ID3D12Device* device, const TPipeLineInitParam *pipeLineInitParam = nullptr) 
		{
			mDevice = device;
			if (pipeLineInitParam) 
			{
				mInitParam = *pipeLineInitParam;
			}
			
			InitViewPort();

			ThrowIfFailed(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));
			ThrowIfFailed(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList)));
			mCommandList->Close();

			mWorkhread.run_loop(&PipeLineTemplate::ProcessResourcePackageFunction, this);

			return true;
		}

		virtual bool PopulateCommandList(TResourcePackage* package, ID3D12GraphicsCommandList* commandlist)
		{
			return true;
		}

		virtual bool PopulateCommandListGC(gc_ptr<TResourcePackage> package, ID3D12GraphicsCommandList* commandlist)
		{
			return true;
		}

		virtual bool PostResourcePackage(TResourcePackage *package)
		{
			package->State.set_state(EResourcePackageState::EPOSTED);
			gc_ptr<TResourcePackage> gc_ptr = get_gc_ptr_from_raw(package);
			mPackageQueue.enqueue(gc_ptr);
			mIdleCount = 0;
			mDrawConVar.notify_one();
			return true;
		}
		
		virtual bool Draw(TResourcePackage* package)
		{
			return true;
		}

	protected:
		ID3D12Device*										mDevice;
		D3D12_VIEWPORT										mViewport;
		D3D12_RECT											mScissorRect;
		std::atomic<uint64_t>								mIdleCount;
		std::mutex											mDrawLock;
		std::condition_variable								mDrawConVar;
		TPipeLineInitParam									mInitParam;
		gc_ptr<Scene>										mScene;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>		mCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	mCommandList;
		ythread<void>										mWorkhread;
		wait_free_queue<gc_ptr<TResourcePackage>>			mPackageQueue;

		virtual void ProcessResourcePackageFunction() 
		{
			gc_ptr<TResourcePackage> package;
			if (mPackageQueue.dequeue(package))
			{
				Draw(package.get_raw_ptr());
				mIdleCount = 0;
			}
			else 
			{
				mIdleCount++;
				std::this_thread::yield();
				if (mIdleCount > 20)
				{
					std::unique_lock<std::mutex> ulock(mDrawLock);
					mDrawConVar.wait(ulock, [this] { return mPackageQueue.size(); });
				}
			}
		}

		virtual void InitViewPort() 
		{
			mViewport = { 0, 0, static_cast<float>(mInitParam.outputWidth), static_cast<float>(mInitParam.outputHeight), 0.0f, 1.0f };
			mScissorRect = {0, 0, static_cast<LONG>(mInitParam.outputWidth), static_cast<LONG>(mInitParam.outputHeight) };
		}

		virtual bool ResetCommand() 
		{
			mCommandAllocator->Reset();
			mCommandList->Reset(mCommandAllocator.Get(), nullptr);
			return true;
		}

	};
}
