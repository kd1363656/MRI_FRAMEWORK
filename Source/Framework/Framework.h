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

//=========================================================
// ファイル用便利関数
//=========================================================
#include "Utility/File/FileUtility.h"

//================================================
// 型情報
//================================================
// 静的ID生成クラス
#include "TypeInfo/StaticID/StaticID.h"

// 型情報判別便利関数
#include "Utility/TypeInfo/TypeInfoUtility.h"

// 型情報登録クラス
#include "TypeInfo/Registry/TypeInfoRegistry.h"

// 型情報登録自動化マクロ
#include "TypeInfo/Macros/TypeInfoMacros.h"

//=========================================================
// タグ
//=========================================================
#include "Tag/TagBase.h"
#include "Tag/WindowStyle/WindowStyleTag.h"

//=========================================================
// コンセプト
//=========================================================
#include "Definition/Concept/IsDerivedBase/IsDerivedBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/IsDerivedTagBaseConcept.h"

//=========================================================
// Tag用便利関数
//=========================================================
#include "Utility/Tag/TagUtility.h"

//=========================================================
// 文字列変換便利関数
//=========================================================
#include "Utility/String/StringUtility.h"

//=========================================================
// Jsonのシリアライズ、デシリアライズ補助関数
//=========================================================
#include "Utility/Json/JsonUtility.h"

//================================================
// Jsonコンバータークラス
//================================================
#include "Converter/Json/JsonConverterBase.h"
#include "Converter/Json/FPS/FPSControllerJsonConverter.h"
#include "Converter/Json/Window/WindowJsonConverter.h"

//=========================================================
// FPS計測クラス
//=========================================================
#include "FPS/FPSController.h"

//=========================================================
// ウィンドウクラス
//=========================================================
#include "Window/Window.h"

//=========================================================
// 描画関係クラス
//=========================================================

// グラフィックスデバイス管理クラス
#include "Graphics/Hardware/Factory/Factory.h"
#include "Graphics/Hardware/Device/Device.h"
#include "Graphics/Hardware/Hardware.h"

// コマンドキュー管理クラス
#include "Graphics/Command/Queue/Fence/Fence.h"
#include "Graphics/Command/Queue/CommandQueueBase.h"
#include "Graphics/Command/Queue/Direct/DirectCommandQueue.h"
#include "Graphics/Command/Queue/Copy/CopyCommandQueue.h"

// コマンドアロケータ管理クラス
#include "Graphics/Command/Allocator/CommandAllocatorBase.h"
#include "Graphics/Command/Allocator/Direct/DirectCommandAllocator.h"
#include "Graphics/Command/Allocator/Copy/CopyCommandAllocator.h"

// コマンドリスト管理クラス
#include "Graphics/Command/List/CommandListBase.h"
#include "Graphics/Command/List/Direct/DirectCommandList.h"
#include "Graphics/Command/List/Copy/CopyCommandList.h"

// ディスクリプタヒープ
#include "Graphics/Resource/Descriptor/Heap/DescriptorHeapBase.h"
#include "Graphics/Resource/Descriptor/Heap/RTV/RTVDescriptorHeap.h"

// ディスクリプタアロケータ
#include "Graphics/Resource/Descriptor/Allocator/DescriptorAllocator.h"

// ディスクリプタベースを継承しているか確認するConcept
#include "Definition/Concept/IsDerivedBase/Descriptor/IsDerivedDescriptorBaseConcept.h"

#include "Graphics/Resource/Descriptor/DescriptorPool.h"

#include "Graphics/GraphicsManager.h"