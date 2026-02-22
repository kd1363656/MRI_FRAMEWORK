#pragma once

// json保存に特化したEnumのようなもの、Enumは定義順序を変えるだけで
// 意味合いが変わるのでバグの温床になるがこのタグシステムは
// 定義順序を変えても絶対に大丈夫な設計、さらにuint32_t型で比較を行うので処理が速い
namespace FWK::Tag
{
	struct TagBase {};
}

FWK_DEFINE_TYPE_INFO_ROOT(FWK::Tag::TagBase);