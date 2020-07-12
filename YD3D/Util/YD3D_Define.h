#pragma once

#include <functional>
#include <d3dx12.h>

#define ALIGN_16	__declspec(align(16))
#define ALIGN(v)	__declspec(align(v))

#define ANY_DESCRIPTOR_HEAP_POS -1 

typedef std::function<void(D3D12_COMMAND_LIST_TYPE, uint64_t)>	GraphicTaskCallbackFunction, CommandQueueCallbackFunction;
typedef std::function<bool(ID3D12GraphicsCommandList*)> GraphicTaskFunction;

enum class ECommandQueueType { ERENDER, ECOPY, ECOMPUTE, ESWAP_CHAIN  };
enum class EResourcePackageState { EINIT, EPOSTED, ERENDERING, ERENDERED };
