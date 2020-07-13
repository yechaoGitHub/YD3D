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

		virtual bool Create(ID3D12Device* device, const TPipeLineInitParam *pipeLineInitParam) 
		{
			mDevice = device;
			mInitParam = *pipeLineInitParam;

			InitViewPort();

			mWorkhread.run_loop(&PipeLineTemplate::ProcessResourcePackageFunction, this);

			return true;
		}

		virtual bool PostResourcePackage(TResourcePackage *package)
		{
			package->State.set_state(EResourcePackageState::EPOSTED);
			gc_ptr<TResourcePackage> gc_ptr = get_gc_ptr_from_raw(package);
			mPackageQueue.enqueue(gc_ptr);
			return true;
		}

		virtual bool Draw(TResourcePackage *package)
		{
			return false;
		}

		virtual bool PopulateCommandList(TResourcePackage *package, ID3D12GraphicsCommandList *commandlist)
		{
			return false;
		}
		
	protected:
		ID3D12Device*								mDevice;
		TPipeLineInitParam							mInitParam;
		gc_ptr<Scene>								mScene;
		ythread<void>								mWorkhread;
		D3D12_VIEWPORT								mViewport;
		D3D12_RECT									mScissorRect;
		wait_free_queue<gc_ptr<TResourcePackage>>	mPackageQueue;

		virtual void ProcessResourcePackageFunction() 
		{
			gc_ptr<TResourcePackage> package;
			if (mPackageQueue.dequeue(package))
			{
				Draw(package.get_raw_ptr());
			}
		}

		virtual void InitViewPort() 
		{
			mViewport = { 0, 0, static_cast<float>(mInitParam.outputWidth), static_cast<float>(mInitParam.outputHeight), 0.0f, 1.0f };
			mScissorRect = {0, 0, static_cast<LONG>(mInitParam.outputWidth), static_cast<LONG>(mInitParam.outputHeight) };
		}

		virtual bool PostToCommandQueue(TResourcePackage *package)
		{
			/*gc_ptr<TResourcePackage> gcPackage = get_gc_ptr_from_raw(package);
			GraphicTaskFunction &&task = std::bind(&PipeLineTemplate::PopulateCommandList, this, package, std::placeholders::_1);
			GraphicTaskCallbackFunction &&packageCallback = std::bind(&TResourcePackage::ResourcePackageCallBack, gcPackage, std::placeholders::_1, std::placeholders::_2);
			GraphicTask::PostGraphicTask(ECommandQueueType::ESWAP_CHAIN, std::move(task), nullptr, std::move(packageCallback));
			
			return true;*/
			return true;
		}
	};
}
