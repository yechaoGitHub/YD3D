#pragma once
#include "YD3D_Header.h"
#include "Graph/CommandQueue.h"
#include "Graph/CommandListAllocator.h"

namespace YD3D
{
	class GraphicTask 
	{
		struct TaskCallbackParam
		{
			TaskCallbackParam() :
				Type(D3D12_COMMAND_LIST_TYPE_DIRECT),
				HasCallback(false)
			{
			}

			D3D12_COMMAND_LIST_TYPE						Type;
			std::vector<ID3D12GraphicsCommandList*>		CommandLists;
			bool										HasCallback;
			GraphicTaskCallbackFunction					Callback;
		};

	public:
		GraphicTask();
		~GraphicTask();

		bool Create(ID3D12Device* device);
		CommandQueue& GetCommandQueue(ECommandQueueType type);

		bool PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue);
		bool PostTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback);
		bool PostTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue);
		bool PostTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback);

		bool WaitForCompletion(ECommandQueueType type, uint64_t fenceValue, bool waitCallback = true);

		static void SetGlobalGraphicTask(GraphicTask &graphicTask);
		static CommandQueue& GetGraphicTaskCommandQueue(ECommandQueueType type);
		static bool PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue);
		static bool PostGraphicTask(ECommandQueueType type, GraphicTaskFunction&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback);
		static bool PostGraphicTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue);
		static bool PostGraphicTask(ECommandQueueType type, std::vector<GraphicTaskFunction>&& task, uint64_t* fenceValue, GraphicTaskCallbackFunction&& callback);
		static bool WaitForGraphicTaskCompletion(ECommandQueueType type, uint64_t fenceValue, bool waitCallback = true);

	private:
		ID3D12Device*			mDevice;
		CommandQueue			mCopyCommandQueue;
		CommandListAllocator	mCopyCommandAllocator;
		CommandQueue			mRenderCommandQueue;
		CommandListAllocator	mRenderCommandAllocator;
		CommandQueue			mComputeCommandQueue;
		CommandListAllocator	mComputeCommandAllocator;
		CommandQueue			mSwapChainCommandQueue;
		CommandListAllocator	mSwapChainCommandAllocator;

		static GraphicTask*		_GLOBAL_GRAPHIC_TASK_;

		CommandListAllocator& GetCommandListAllocator(ECommandQueueType type);
		D3D12_COMMAND_LIST_TYPE SwitchToDX12Type(ECommandQueueType type);
		void GraphicTaskDefaultCallback(TaskCallbackParam& param, D3D12_COMMAND_LIST_TYPE type, uint64_t fence);
		void FreeCommandList(D3D12_COMMAND_LIST_TYPE type, std::vector<ID3D12GraphicsCommandList*>& vecCommandList);
	};

};
