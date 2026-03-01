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
#include "Utility/TypeInfo/TypeInfoUtility.h"
#include "TypeInfo/Register/TypeInfoRegister.h"
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

//=========================================================
// FPS計測クラス
//=========================================================
#include "FPS/FPSController.h"

//================================================
// ウィンドウ
//================================================
#include "Window/Window.h"