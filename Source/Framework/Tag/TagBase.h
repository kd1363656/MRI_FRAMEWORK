#pragma once

// "json"保存に特化した"Enum"のようなもの、"Enum"は定義順序を変えるだけで
// 意味合いが変わるのでバグの温床になるがこのタグシステムは
// 定義順序を変えても絶対に大丈夫な設計、さらに"uint32_t"型で比較を行うので処理が速い
namespace FWK::Tag
{
	struct TagBase {};
}

FWK_DEFINE_TYPE_INFO_ROOT(FWK::Tag::TagBase);