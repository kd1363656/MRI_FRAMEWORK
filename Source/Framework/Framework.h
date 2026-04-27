#pragma once

//===============================================================================
// 定義
//===============================================================================

// NLOHMNN_JSON_SERIALIZE_ENUMマクロ
#include "Definition/Macros/Json/JsonSerializeEnumMacros.h"

// 型エイリアス
#include "Definition/Type/Alias/TypeAlias.h"

// StaticTypeIDを使う際に他の場所でも使う定数をまとめたもの
#include "Definition/Constant/StaticTypeID/StaticTypeIDConstant.h"

// TypeTag用定数
#include "Definition/Constant/TypeTag/TypeTagConstant.h"

// Windowの縦幅横幅に関する定数をまとめたもの
#include "Definition/Constant/WindowSize/WindowSizeConstant.h"

// FPS関係の定数
#include "Definition/Constant/FPS/FPSConstant.h"

// Graphics関係の定数
#include "Definition/Constant/Graphics/Command/Queue/Fence/FenceConstant.h"
#include "Definition/Constant/Graphics/Resource/Descriptor/DescriptorConstant.h"
#include "Definition/Constant/Graphics/SwapChain/SwapChainConstant.h"
#include "Definition/Constant/Graphics/Render/RootSignature/RootSignatureConstant.h"
#include "Definition/Constant/Graphics/Resource/Texture/TextureConstant.h"
#include "Definition/Constant/Graphics/Resource/Heap/HeapConstant.h"

// ファイル関係の定数
#include "Definition/Constant/FileExtension/FileExtensionConstant.h"

// 文字列ハッシュ構造体
#include "Definition/Struct/StringHash/StringHashStruct.h"

// WindowCONFIG構造体
#include "Definition/Struct/Window/WindowStruct.h"

// バックバッファ構造体
#include "Definition/Struct/Graphics/SwapChain/SwapChainStruct.h"

// ルートシグネチャ用構造体
#include "Definition/Struct/Graphics/Render/RootSignature/RootSignatureStruct.h"

// テクスチャ用構造体
#include "Definition/Struct/Graphics/Resource/Texture/TextureStruct.h"

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
#include "Tag/Graphics/Render/RootSignature/RootSignatureTag.h"
#include "Tag/Graphics/Render/PipelineState/PipelineStateTag.h"

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
#include "JsonConverter/Window/WindowJsonConverter.h"
#include "Window/Window.h"

//===============================================================================
// FPS計測クラス
//===============================================================================
#include "JsonConverter/FPS/FPSControllerJsonConverter.h"
#include "FPS/FPSController.h"

//===============================================================================
// 描画管理クラス
//===============================================================================

// グラフィックスデバイス管理クラス
#include "Graphics/Hardware/Factory.h"
#include "Graphics/Hardware/Device.h"

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
#include "Graphics/Resource/Descriptor/Heap/SRV/SRVDescriptorHeap.h"

// ディスクリプタヒープスロットアロケータ
#include "Graphics/Resource/Descriptor/Allocator/DescriptorHeapIndexAllocator.h"

// ディスクリプタベースを継承しているかどうかを確認するコンセプト
#include "Definition/Concept/IsDerivedBase/Descriptor/IsDerivedDescriptorBaseConcept.h"

// ディスクリプタプール
#include "JsonConverter/Graphics/Resource/Descriptor/DescriptorPoolJsonConverter.h"
#include "Graphics/Resource/Descriptor/DescriptorPool.h"

// アップロードバッファー
#include "Graphics/Resource/Buffer/UploadBuffer.h"

// アップロードシステム
#include "Graphics/Resource/Upload/UploadSystem.h"

// テクスチャ
#include "Graphics/Resource/Texture/Loader/TextureLoader.h"
#include "Graphics/Resource/Texture/Uploader/TextureUploader.h"
#include "Graphics/Resource/Texture/TextureSystem.h"

// リソースコンテキスト
#include "JsonConverter/Graphics/Resource/ResourceContextJsonConverter.h"
#include "Graphics/Resource/ResourceContext.h"

// スワップチェイン
#include "JsonConverter/Graphics/SwapChain/SwapChainJsonConverter.h"
#include "Graphics/SwapChain/SwapChain.h"

// フレームリソースクラス
#include "Graphics/Render/Frame/FrameResource.h"

// レンダーエリアクラス
#include "Graphics/Render/Rasterizer/RenderArea.h"

// ルートシグネチャ
#include "JsonConverter/Graphics/Render/RootSignature/RootSignatureJsonConverter.h"
#include "Graphics/Render/RootSignature/RootSignature.h"

// DXCコンパイラクラス
#include "Graphics/Shader/ShaderCompiler.h"

// シェーダークラス
#include "JsonConverter/Graphics/Shader/ShaderJsonConverter.h"
#include "Graphics/Shader/Shader.h"

// パイプラインステート
#include "JsonConverter/Graphics/Render/Pipeline/PipelineStateJsonConverter.h"
#include "Graphics/Render/Pipeline/PipelineState.h"

// レンダラー
#include "JsonConverter/Graphics/Render/RendererJsonConverter.h"
#include "Graphics/Render/Renderer.h"

// グラフィックスマネージャー
#include "JsonConverter/Graphics/GraphicsManagerJsonConverter.h"
#include "Graphics/GraphicsManager.h"

// シーン
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"