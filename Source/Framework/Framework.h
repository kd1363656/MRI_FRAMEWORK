#pragma once

//===============================================
// 型エイリアス
//===============================================
#include "Definition/Type/TypeDEF.h"

//===============================================
// シングルトンを安全に扱うためのクラス
//===============================================
#include "Singleton/SingletonBase.h"

//=========================================================
// ファイル読み込み、書き込み用便利関数
//=========================================================
#include "Utility/FileIO/FileIOUtility.h"

//=========================================================
// 文字列変換便利関数
//=========================================================
#include "Utility/String/StringUtility.h"

//===============================================
// 定義、定数
//===============================================
#include "Definition/Constant/CommonConstant.h"
#include "Definition/Struct/CommonStruct.h"

//=========================================================
// 静的型ID管理クラス
//=========================================================
#include "StaticID/StaticID.h"

//=========================================================
// 型情報定義、管理クラス及び便利関数
//=========================================================
#include "TypeInfo/TypeInfo.h"
#include "Utility/TypeInfo/TypeInfoUtility.h"
#include "TypeInfo/Registry/TypeInfoRegistry.h"

//=========================================================
// タグ構造体
//=========================================================
#include "Tag/TagBase.h"
#include "Tag/WindowStyle/WindowStyleTag.h"

//=========================================================
// 基底クラスから派生しているかチェックするコンセプト
//=========================================================
#include "Definition/Concept/IsDerivedBase/IsDerivedBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/IsDerivedWindowStyleTagBaseConcept.h"

//=========================================================
// Jsonのシリアライズ、デシリアライズ補助関数
//=========================================================
#include "Utility/Json/JsonUtility.h"

//=========================================================
// jsonコンバータークラス
//=========================================================
#include "Converter/Json/JsonConverterBase.h"
#include "Converter/Json/FPS/FPSControllerJsonConverter.h"
#include "Converter/Json/Window/WindowJsonConverter.h"
#include "Converter/Json/Render/RendererJsonConverter.h"
#include "Converter/Json/Render/Hardware/HardwareJsonConverter.h"

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
#include "Render/Hardware/Hardware.h"
#include "Render/Command/Base/CommandContextBase.h"
#include "Render/Command/Base/Queue/CommandQueueBase.h"
#include "Render/Command/Base/Allocator/CommandAllocatorBase.h"
#include "Render/Command/Base/List/CommandListBase.h"

#include "Render/Renderer.h"