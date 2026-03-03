#pragma once

//================================================
// 定義
//================================================
#include "Definition/Type/TypeAlias.h"
#include "Definition/Constant/CommonConstant.h"
#include "Definition/Struct/CommonStruct.h"

//================================================
// シングルトンを安全に扱うための基底クラス
//================================================
#include "Singleton/SingletonBase.h"

//================================================
// 型情報
//================================================
#include "TypeInfo/StaticID/StaticID.h"
#include "TypeInfo/TypeInfo.h"

// 型情報判別便利関数
#include "Utility/TypeInfo/TypeInfoUtility.h"

// 型情報登録クラス
#include "TypeInfo/Registry/TypeInfoRegistry.h"

// 型情報定義マクロ
#include "TypeInfo/Macros/TypeInfoMacros.h"

//=========================================================
// タグ
//=========================================================
#include "Tag/TagBase.h"
#include "Tag/WindowStyle/WindowStyleTag.h"

//=========================================================
// 文字列変換便利関数
//=========================================================
#include "Utility/String/StringUtility.h"

//=========================================================
// ファイル読み込み、書き込み用便利関数
//=========================================================
#include "Utility/FileIO/FileIOUtility.h"

//=========================================================
// Jsonのシリアライズ、デシリアライズ補助関数
//=========================================================
#include "Utility/Json/JsonUtility.h"

//================================================
// Jsonコンバーター
//================================================
#include "Converter/Json/JsonConverterBase.h"
#include "Converter/Json/Window/WindowJsonConverter.h"
#include "Converter/Json/FPS/FPSJsonConverter.h"
#include "Converter/Json/Graphics/Hardware/Device/DeviceJsonConverter.h"
#include "Converter/Json/Graphics/SwapChain/SwapChainJsonConverter.h"

//=========================================================
// FPS計測クラス
//=========================================================
#include "FPS/FPSController.h"

//================================================
// ウィンドウ
//================================================
#include "Window/Window.h"

//================================================
// 描画関係
//================================================

// ハードウェア管理クラス
#include "Graphics/Hardware/Factory/Factory.h"
#include "Graphics/Hardware/Device/Device.h"
#include "Graphics/Hardware/Hardware.h"

// 描画関係クラス
#include "Graphics/Command/Queue/CommandQueueBase.h"
#include "Graphics/Command/Allocator/CommandAllocatorBase.h"
#include "Graphics/Command/List/CommandListBase.h"
#include "Graphics/Command/Queue/Direct/DirectCommandQueueh.h"
#include "Graphics/Command/Allocator/Direct/DirectCommandAllocator.h"
#include "Graphics/Command/List/Direct/DirectCommandList.h"
#include "Graphics/Render/Frame/FrameResource.h"
#include "Graphics/Render/Renderer.h"

// 描画リソース関係クラス
#include "Graphics/Resource/Descriptor/Heap/DescriptorHeapBase.h"
#include "Graphics/Resource/Descriptor/Heap/RTV/RTVDescriptorHeap.h"
#include "Graphics/Resource/Descriptor/DescriptorHeapContext.h"
#include "Graphics/Resource/ResourceContext.h"

// スワップチェイン
#include "Graphics/SwapChain/SwapChain.h"

#include "Graphics/GraphicsManager.h"