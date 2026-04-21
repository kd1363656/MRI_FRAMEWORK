#pragma once

namespace FWK::Tag
{
	struct WindowStyleTagBase : TagBase { FWK_DEFINE_TYPE_INFO_TAG(WindowStyleTagBase, TagBase); };

	struct WindowStyleBorderlessFullScreenTag final : WindowStyleTagBase { FWK_DEFINE_TYPE_INFO_TAG(WindowStyleBorderlessFullScreenTag, WindowStyleTagBase); };
	struct WindowStyleNormalTag               final : WindowStyleTagBase { FWK_DEFINE_TYPE_INFO_TAG(WindowStyleNormalTag,               WindowStyleTagBase); };
}