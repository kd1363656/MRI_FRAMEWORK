#pragma once

namespace FWK::Tag
{
	struct WindowStyleTagBase : TagBase {};

	struct WindowStyleFullScreenTag final : WindowStyleTagBase {};
	struct WindowStyleNormalTag     final : WindowStyleTagBase {};
}

FWK_DEFINE_TYPE_INFO(FWK::Tag::WindowStyleTagBase, FWK::Tag::TagBase);

FWK_DEFINE_TYPE_INFO(FWK::Tag::WindowStyleFullScreenTag, FWK::Tag::WindowStyleTagBase);
FWK_DEFINE_TYPE_INFO(FWK::Tag::WindowStyleNormalTag,     FWK::Tag::WindowStyleTagBase);