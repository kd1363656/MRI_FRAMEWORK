#pragma once

// ========================================
//  Windows関係ヘッダー
// ========================================
#pragma comment(lib, "winmm.lib")

#define NOMINMAX
#include <Windows.h>
#include <wrl/client.h>
#include <cassert>

// ========================================
//  STL
// ========================================
#include <string>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <format>
#include <memory>

// ========================================
//  外部ライブラリ
// ========================================
#include <nlohmann/json.hpp>
#include <d3d12.h>
#include <dxgi1_6.h>s
#include <DirectXMath.h>
#include <directx-dxc/dxcapi.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// ========================================
//  フレームワーク
// ========================================
#include "Framework/Framework.h"