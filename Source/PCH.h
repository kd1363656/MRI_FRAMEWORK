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
#include <cstdint>
#include <string>
#include <unordered_map>
#include <limits>
#include <filesystem>
#include <fstream>
#include <thread>

//================================================
// 外部ライブラリ
//================================================
// json
#include <Json/Inc/nlohmann/json.hpp>

//================================================
// フレームワーク
//================================================
#include "Framework/Framework.h"

//================================================
// リンカー設定
//================================================
#pragma comment(lib, "winmm.lib")