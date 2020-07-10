#pragma once

#include <functional>
#include <d3dx12.h>

#define ALIGN_16	__declspec(align(16))
#define ALIGN(v)	__declspec(align(v))

typedef std::function<void(void)>				GraphicTaskCallBackFunction;
typedef std::function<bool(ID3D12GraphicsCommandList*)> GraphicTaskFunction;

enum class ECommandQueueType { ERENDER, ECOPY, ECOMPUTE, ESWAP_CHAIN  };
