#pragma once

//================================================
// Windows
//================================================
#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
#include <cassert>

//================================================
// STL
//================================================
#include <string>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <format>
#include <memory>
#include <queue>

//================================================
// 外部ライブラリ
//================================================
// json
#include <nlohmann/json.hpp>

// DirectX12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3DX12/d3dx12.h>
#include <DXCCompiler/Inc/dxcapi.h>
#include <DirectXTex/Inc/DirectXTex.h>

//================================================
// フレームワーク
//================================================
#include "Framework/Framework.h"

//================================================
// リンカー設定
//================================================
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")