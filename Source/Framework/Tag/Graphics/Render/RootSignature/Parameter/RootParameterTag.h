#pragma once

namespace FWK::Tag
{
	struct RootParameterTagBase : public TagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterTagBase, TagBase); };

	// テクスチャ関係
	struct RootParameterSpriteBaseColorTextureTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterSpriteBaseColorTextureTag, RootParameterTagBase); };
	struct RootParameterCBSpriteDrawTag			  final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpriteDrawTag,			 RootParameterTagBase); };
	struct RootParameterCBSpritePassTag			  final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBSpritePassTag,			 RootParameterTagBase); };

	// カメラ関係
	struct RootParameterCBCameraTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBCameraTag, RootParameterTagBase); };

	// Model共通
	struct RootParameterCBModelObjectTag   final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelObjectTag,   RootParameterTagBase); };
	struct RootParameterCBModelMaterialTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterCBModelMaterialTag, RootParameterTagBase); };
	
	struct RootParameterModelVertexBufferTag      final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterModelVertexBufferTag,      RootParameterTagBase); };
	struct RootParameterModelMeshletBufferTag     final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterModelMeshletBufferTag,     RootParameterTagBase); };
	struct RootParameterModelUniqueVertexIndexTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterModelUniqueVertexIndexTag, RootParameterTagBase); };
	struct RootParameterModelPrimitiveIndexTag	  final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterModelPrimitiveIndexTag,    RootParameterTagBase); };

	struct RootParameterModelBaseColorTextureTag final : public RootParameterTagBase { FWK_DEFINE_TYPE_INFO_TAG(RootParameterModelBaseColorTextureTag, RootParameterTagBase); };
}