#pragma once
//
//===============================================================================
// 定義
//===============================================================================
// TypeI
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

// File関係の定数
#include "Definition/Constant/FileExtension/FileExtensionConstant.h"

// Enum
#include "Definition/Enum/Type/Trait/PTR/TypeTraitPTREnum.h"
#include "Definition/Enum/Graphics/Resource/Texture/DefaultTextureTypeEnum.h"

// 定数
#include "Definition/Constant/Graphics/GraphicsConstant.h"
#include "Definition/Constant/Graphics/Command/CommandConstant.h"
#include "Definition/Constant/Graphics/Command/Queue/Fence/FenceConstant.h"
#include "Definition/Constant/Graphics/Resource/ResourceConstant.h"
#include "Definition/Constant/Graphics/Resource/Descriptor/DescriptorConstant.h"
#include "Definition/Constant/Graphics/Resource/Storage/Allocator/StorageIDAllocatorConstant.h"
#include "Definition/Constant/Graphics/Resource/Storage/StorageConstant.h"
#include "Definition/Constant/Graphics/SwapChain/SwapChainConstant.h"
#include "Definition/Constant/Graphics/Render/RootSignature/RootSignatureConstant.h"
#include "Definition/Constant/Graphics/Render/Pipeline/PipelineStateConstant.h"
#include "Definition/Constant/Graphics/Render/Light/LightSystemConstant.h"
#include "Definition/Constant/Graphics/Resource/Texture/TextureConstant.h"
#include "Definition/Constant/Graphics/Resource/Texture/RenderTarget/RenderTargetTextureConstant.h"
#include "Definition/Constant/Graphics/Resource/Texture/DepthStencil/DepthStencilTextureConstant.h"
#include "Definition/Constant/Graphics/Resource/Heap/HeapConstant.h"
#include "Definition/Constant/Graphics/Resource/Buffer/Constant/ConstantBufferConstant.h"
#include "Definition/Constant/Graphics/Resource/Buffer/UploadBufferConstant.h"
#include "Definition/Constant/Graphics/Resource/Model/ModelConstant.h"
#include "Definition/Constant/Graphics/Resource/Model/Meshoptimizer/MeshoptimizerConstant.h"
#include "Definition/Constant/Graphics/Render/RendererConstant.h"
#include "Definition/Constant/Graphics/Resource/Upload/UploadSystemConstant.h"

// 構造体
#include "Definition/Struct/Graphics/SwapChain/SwapChainStruct.h"
#include "Definition/Struct/Graphics/Render/RootSignature/RootSignatureStruct.h"
#include "Definition/Struct/Graphics/Render/Light/LightStruct.h"
#include "Definition/Struct/Graphics/Resource/ResourceStruct.h"
#include "Definition/Struct/Graphics/Resource/Buffer/BufferStruct.h"
#include "Definition/Struct/Graphics/Resource/Storage/StorageStruct.h"
#include "Definition/Struct/Graphics/Resource/Texture/TextureStruct.h"
#include "Definition/Struct/Graphics/Resource/Buffer/Constant/Sprite/SpriteConstantBufferStruct.h"
#include "Definition/Struct/Graphics/Resource/Buffer/Constant/Camera/CameraConstantBufferStruct.h"
#include "Definition/Struct/Graphics/Resource/Buffer/Constant/Light/LightConstantBufferStruct.h"
#include "Definition/Struct/Graphics/Resource/Buffer/Constant/Model/ModelConstantBufferStruct.h"
#include "Definition/Struct/StringHash/StringHashStruct.h"
#include "Definition/Struct/Window/WindowStruct.h"

// TypeTrait
#include "Definition/Type/Trait/PTR/TypeTraitPTR.h"

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
#include "Tag/Graphics/Render/RootSignature/Parameter/RootParameterTag.h"
#include "Tag/Graphics/Render/PipelineState/PipelineStateTag.h"
#include "Tag/Graphics/Render/Frame/Scene/SceneTextureTag.h"
#include "Tag/Graphics/Render/Graph/Pass/RenderGraphPassTag.h"

//===============================================================================
// コンセプト
//===============================================================================
#include "Definition/Concept/IsDerivedBase/IsDerivedBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/IsDerivedTagBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Struct/Graphics/Resource/Storage/IsDerivedAssetRecordBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/Graphics/Render/Pipeline/IsDerivedPipelineStateTagBaseConcept.h"
#include "Definition/Concept/IsDerivedBase/Tag/Graphics/Render/RootSignature/Parameter/IsDerivedRootParameterTagBaseConcept.h"
#include "Definition/Concept/IsSmartPTR/IsSmartPTRConcept.h"

//===============================================================================
// 凡庸ファクトリークラス
//===============================================================================
#include "Factory/GenericFactory.h"

// ファクトリー登録自動化マクロ
#include "Definition/Macros/Factory/FactoryMacros.h"

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
// 計算便利関数
//===============================================================================
#include "Utility/Math/MathUtility.h"

//===============================================================================
// ウィンドウクラス
//===============================================================================
#include "Converter/Json/Window/WindowJsonConverter.h"
#include "Window/Window.h"

//===============================================================================
// FPS計測クラス
//===============================================================================
#include "Converter/Json/FPS/FPSControllerJsonConverter.h"
#include "FPS/FPSController.h"

//===============================================================================
// Binaryファイルクラス
//===============================================================================
#include "Converter/Binary/BinaryFileConverterBase.h"

//===============================================================================
// 描画管理クラス
//===============================================================================

// グラフィックスデバイス管理クラス
#include "Graphics/Hardware/Factory.h"
#include "Graphics/Hardware/Device.h"

// ディスクリプタヒープ
#include "Graphics/Resource/Descriptor/Heap/DescriptorHeapBase.h"
#include "Graphics/Resource/Descriptor/Heap/RTV/RTVDescriptorHeap.h"
#include "Graphics/Resource/Descriptor/Heap/SRV/SRVDescriptorHeap.h"
#include "Graphics/Resource/Descriptor/Heap/DSV/DSVDescriptorHeap.h"

// ルートシグネチャ
#include "Converter/Json/Graphics/Render/RootSignature/RootSignatureJsonConverter.h"
#include "Graphics/Render/RootSignature/RootSignature.h"

// アップロードバッファー
#include "Graphics/Resource/Buffer/UploadBuffer.h"

// 定数バッファ
#include "Converter/Json/Graphics/Resource/Buffer/Constant/ConstantBufferJsonConverter.h"
#include "Definition/Concept/IsDerivedBase/Graphics/Resource/Buffer/Constant/IsDerivedConstantBufferBaseConcept.h"
#include "Graphics/Resource/Buffer/Constant/ConstantBufferBase.h"

#include "Definition/Type/Alias/Factory/Shared/Graphics/Resource/Buffer/Constant/TypeAliasShaderFactoryConstantBuffer.h"

// 定数バッファ
#include "Graphics/Resource/Buffer/Constant/Sprite/Object/SpriteObjectConstantBuffer.h"
#include "Graphics/Resource/Buffer/Constant/Sprite/Pass/SpritePassConstantBuffer.h"
#include "Graphics/Resource/Buffer/Constant/Camera/CameraConstantBuffer.h"
#include "Graphics/Resource/Buffer/Constant/Model/Object/ModelObjectConstantBuffer.h"
#include "Graphics/Resource/Buffer/Constant/Light/LightConstantBuffer.h"

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

// Storage
#include "Converter/Json/Graphics/Resource/Storage/Allocator/StorageIDAllocatorJsonConverter.h"
#include "Graphics/Resource/Storage/Allocator/StorageIDAllocator.h"
#include "Converter/Json/Graphics/Resource/Storage/AssetStorageJsonConverter.h"
#include "Graphics/Resource/Storage/AssetStorage.h"

// ディスクリプタベースを継承しているかどうかを確認するコンセプト
#include "Definition/Concept/IsDerivedBase/Graphics/Resource/Descriptor/IsDerivedDescriptorBaseConcept.h"

// ディスクリプタプール
#include "Converter/Json/Graphics/Resource/Descriptor/DescriptorPoolJsonConverter.h"
#include "Graphics/Resource/Descriptor/DescriptorPool.h"

// メモリアロケータクラス
#include "Graphics/Resource/Memory/GPUMemoryAllocator.h"

// アップロードシステム用構造体
#include "Definition/Struct/Graphics/Resource/Upload/UploadSystemStruct.h"

// テクスチャ一括アップロード用構造体
#include "Definition/Struct/Graphics/Resource/Texture/Builder/TextureBatchUploadRecordBuilderStruct.h"

// モデル用構造体
#include "Definition/Struct/Graphics/Resource/Model/ModelStruct.h"
#include "Definition/Struct/Graphics/Resource/Model/Static/StaticModelStruct.h"

// ライトシステム
#include "Graphics/Render/Light/LightSystem.h"

// 描画コマンド構造体
#include "Definition/Struct/Graphics/Render/Command/DrawCommandStruct.h"

// アップロードシステム用TypeAlias
#include "Definition/Type/Alias/Graphics/Resource/Upload/TypeAliasUploadSystem.h"

// アップロードシステム
#include "Converter/Json/Graphics/Resource/Upload/UploadSystemJsonConverter.h"
#include "Graphics/Resource/Upload/UploadSystem.h"

// テクスチャ
#include "Graphics/Resource/Texture/Loader/TextureLoader.h"
#include "Graphics/Resource/Texture/Builder/TextureBatchUploadRecordBuilder.h"
#include "Graphics/Resource/Texture/Builder/DefaultTextureBuilder.h"
#include "Converter/Json/Graphics/Resource/Texture/TextureSystemJsonConverter.h"
#include "Graphics/Resource/Texture/Releaser/TextureRecordReleaser.h"
#include "Converter/Json/Graphics/Resource/Texture/DepthStencil/DepthStencilTextureJsonConverter.h"
#include "Graphics/Resource/Texture/DepthStencil/DepthStencilTexture.h"
#include "Converter/Json/Graphics/Resource/Texture/RenderTarget/RenderTargetTextureJsonConverter.h"
#include "Graphics/Resource/Texture/RenderTarget/RenderTargetTexture.h"
#include "Graphics/Resource/Texture/TextureSystem.h"
#include "Graphics/Resource/Texture/Texture.h"

// モデル
#include "Graphics/Resource/Model/FBXLoader/FBXModelLoaderBase.h"
#include "Converter/Binary/Graphics/Resource/Model/Static/StaticModelBinaryConverter.h"
#include "Graphics/Resource/Model/FBXLoader/Static/StaticModelFBXLoader.h"
#include "Graphics/Resource/Model/MeshOptimizer/Static/StaticMeshOptimizer.h"
#include "Graphics/Resource/Model/MeshOptimizer/Static/StaticModelMeshletBuilder.h"
#include "Converter/Json/Graphics/Resource/Model/Static/StaticModelSystemJsonConverter.h"
#include "Graphics/Resource/Model/Builder/Static/StaticModelBatchUploadRecordBuilder.h"
#include "Graphics/Resource/Model/Static/Releaser/StaticModelReleaser.h"
#include "Graphics/Resource/Model/Static/StaticModelSystem.h"
#include "Graphics/Resource/Model/Static/StaticModel.h"

// リソースコンテキスト
#include "Converter/Json/Graphics/Resource/ResourceContextJsonConverter.h"
#include "Graphics/Resource/ResourceContext.h"

// スワップチェイン
#include "Converter/Json/Graphics/SwapChain/SwapChainJsonConverter.h"
#include "Graphics/SwapChain/SwapChain.h"

// シーンテクスチャ用構造体
#include "Definition/Struct/Graphics/Render/Frame/Scene/SceneTextureStruct.h"

// フレームリソースクラス
#include "Converter/Json/Graphics/Render/Frame/Scene/SceneTextureJsonConverter.h"
#include "Graphics/Render/Frame/Scene/SceneTexture.h"
#include "Converter/Json/Graphics/Render/Frame/FrameResourceJsonConverter.h"
#include "Graphics/Render/Frame/FrameResource.h"

// レンダーエリアクラス
#include "Graphics/Render/Rasterizer/RenderArea.h"

// DXCコンパイラクラス
#include "Graphics/Shader/ShaderCompiler.h"

// シェーダークラス
#include "Converter/Json/Graphics/Shader/ShaderJsonConverter.h"
#include "Graphics/Shader/Shader.h"

// パイプラインステート
#include "Converter/Json/Graphics/Render/Pipeline/PipelineStateJsonConverter.h"
#include "Graphics/Render/Pipeline/PipelineState.h"

// カメラ
#include "Graphics/Render/Camera/Camera.h"

// 描画コマンドクラス
#include "Graphics/Render/Command/DrawCommandBase.h"

// 描画コマンドの基底クラスを継承しているかどうかを確認するConcept
#include "Definition/Concept/IsDerivedBase/Graphics/Renderer/Command/IsDerivedIDrawCommandConcept.h"

// レンダーグラフ構造体
#include "Definition/Struct/Graphics/Render/Graph/RenderGraphStruct.h"
#include "Definition/Struct/Graphics/Render/Graph/Resource/RenderGraphResourceStruct.h"

// レンダーパス
#include "Graphics/Render/Graph/Pass/IRenderGraphPass.h"
#include "Graphics/Render/Graph/Pass/RenderGraphPassBase.h"
#include "Graphics/Render/Graph/Resource/RenderGraphResourceRegistry.h"

// レンダラー
#include "Converter/Json/Graphics/Render/RendererJsonConverter.h"
#include "Graphics/Render/Renderer.h"

// 描画コマンドクラス
#include "Graphics/Render/Command/DrawRequestCommandBase.h"

// 描画コマンドクラス用ファクトリTypeAlias
#include "Definition/Type/Alias/Factory/Shared/Graphics/Render/Command/TypeAliasShaderFactoryDrawCommand.h"

// 描画コマンドクラス
#include "Graphics/Render/Command/Sprite/Standard/DrawSpriteStandardCommand.h"
#include "Graphics/Render/Command/Model/Static/Standard/DrawStaticModelStandardCommandBase.h"
#include "Graphics/Render/Command/Model/Static/Standard/UnLit/DrawStaticModelUnLitStandardCommand.h"
#include "Graphics/Render/Command/Model/Static/Standard/Lit/DrawStaticModelLitStandardCommand.h"

// グラフィックスマネージャー
#include "Converter/Json/Graphics/GraphicsManagerJsonConverter.h"
#include "Graphics/GraphicsManager.h"

// エディターマネージャー
#include "Editor/EditorManager.h"

// ゲームオブジェクト
#include "GameObject/GameObject.h"

// シーン
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"