#pragma once

namespace FWK::Tag
{
	struct WindowStyleTagBase : TagBase { FWK_DEFINE_TYPE_INFO(WindowStyleTagBase, TagBase); };

	struct WindowStyleFullScreenTag final : WindowStyleTagBase { FWK_DEFINE_TYPE_INFO(WindowStyleFullScreenTag, WindowStyleTagBase); };
	struct WindowStyleNormalTag     final : WindowStyleTagBase { FWK_DEFINE_TYPE_INFO(WindowStyleNormalTag,		WindowStyleTagBase); };  
}