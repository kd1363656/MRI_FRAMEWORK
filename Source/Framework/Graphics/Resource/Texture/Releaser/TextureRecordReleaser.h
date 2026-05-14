#pragma once

namespace FWK::Graphics
{
	// ※注意
	// 参照を保持する関係上メンバ変数にすると未定義動作を招く可能性がある
	// ローカル変数として扱うこと前提のクラスである
	class TextureRecordReleaser final
	{
	public:

		explicit  TextureRecordReleaser(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);
				 ~TextureRecordReleaser();

		bool ReleaseRecord(const std::weak_ptr<Struct::TextureRecord>& a_textureRecord) const;

	private:

		DescriptorPool<SRVDescriptorHeap>& m_srvDescriptorPool;
	};
}