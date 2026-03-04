#pragma once

//================================================
// Windows
//================================================
#pragma comment(lib, "winmm.lib")

#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
#include <cassert>

//================================================
// STL
//================================================
#include <string>
#include <cstddef>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <thread>
#include <fstream>
#include <chrono>
#include <format>

//================================================
// 外部ライブラリ
//================================================
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// json
#include <nlohmann/json.hpp>

// DirectX12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3DX12/d3dx12.h>
#include <DXCCompiler/Inc/dxcapi.h>

//================================================
// フレームワーク
//================================================
#include "Framework/Framework.h"