#pragma once

//===============================================================================
// 定義
//===============================================================================

// 型エイリアス
#include "Definition/Type/Alias/TypeAlias.h"

// StaticTypeIDを使う際に他の場所でも使う定数をまとめたもの
#include "Definition/Constant/StaticTypeID/StaticTypeIDConstant.h"

// Windowの縦幅横幅に関する定数をまとめたもの
#include "Definition/Constant/WindowSize/WindowSizeConstant.h"

// FPS関係の定数
#include "Definition/Constant/FPS/FPSConstant.h"

// 文字列ハッシュ構造体
#include "Definition/Struct/StringHash/StringHashStruct.h"

// WindowCONFIG構造体
#include "Definition/Struct/WindowCONFIG/WindowCONFIGStruct.h"

//===============================================================================
// シングルトンを安全に扱うための基底クラス
//===============================================================================
#include "Singleton/SingletonBase.h"

//===============================================================================
// 型情報
//===============================================================================

// 静的型ID生成クラス
#include "TypeINFO/StaticTypeID/StaticTypeIDGenerator.h"

// 構造体
#include "Definition/Struct/TypeINFO/TypeINFOStruct.h"

// 型情報用便利関数
#include "Utility/TypeINFO/TypeINFOUtility.h"

// 型情報登録クラス
#include "TypeINFO/Registry/TypeINFORegistry.h"

// 型情報用マクロ
#include "Definition/Macros/TypeINFO/TypeINFOMacros.h"
#include "Definition/Macros/TypeINFO/Tag/TagTypeINFOMacros.h"

//===============================================================================
// タグ
//===============================================================================
#include "Tag/TagBase.h"
#include "Tag/Window/WindowStyleTag.h"

//===============================================================================
// コンセプト
//===============================================================================
#include "Definition/Concept/IsDerivedBase/IsDerivedBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/IsDerivedTagBaseConcept.h"

//===============================================================================
// Jsonのシリアライズ、デシリアライズ補助関数
//===============================================================================
#include "Utility/Json/JsonUtility.h"

//===============================================================================
// Tag便利関数
//===============================================================================
#include "Utility/Tag/TagUtility.h"

//===============================================================================
// ファイル用便利関数
//===============================================================================
#include "Utility/File/FileUtility.h"

//===============================================================================
// 文字列変換便利関数
//===============================================================================
#include "Utility/String/StringUtility.h"

//===============================================================================
// ウィンドウクラス
//===============================================================================
#include "Converter/Window/WindowJsonConverter.h"
#include "Window/Window.h"

//===============================================================================
// FPS計測クラス
//===============================================================================
#include "Converter/FPS/FPSControllerJsonConverter.h"
#include "FPS/FPSController.h"

//===============================================================================
// 描画管理クラス
//===============================================================================

// グラフィックスデバイス管理クラス
#include "Graphics/Hardware/Factory.h"
#include "Graphics/Hardware/Device.h"

// グラフィックスマネージャー
#include "Graphics/GraphicsManager.h"