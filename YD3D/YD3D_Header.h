#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <assert.h>
#include <d3dx12.h>
#include <dxgi1_5.h>
#include <DirectXColors.h>

#include <stdint.h>
#include <string>
#include <vector>
#include <array>
#include <set>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <type_traits>

#include "garbage_collection.h"
#include "gc_ptr.hpp"
#include "gc_allocator.hpp"

#include "Util/YD3D_Define.h"
#include "Util/YD3D_Util.h"
#include "Util/wait_free_queue.hpp"
#include "Util/ythread.hpp"
#include "Util/ystate.hpp"

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
