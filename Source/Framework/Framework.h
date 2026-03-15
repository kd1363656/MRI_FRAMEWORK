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