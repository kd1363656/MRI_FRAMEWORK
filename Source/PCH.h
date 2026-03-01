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
#include <nlohmann/json.hpp>

//================================================
// フレームワーク
//================================================
#include "Framework/Framework.h"