#pragma once

namespace FWK::Graphics 
{
	// ※注意
	// 参照を保持する関係上メンバ変数にすると未定義動作を招く可能性がある
	// ローカル変数として扱うこと前提のクラスである
	class StaticModelRecordReleaser final
	{
	public:

		explicit StaticModelRecordReleaser(DescriptorPool<SRVDescriptorHeap>& a_srvDescriptorPool);
				~StaticModelRecordReleaser();

		bool ReleaseRecord(const std::weak_ptr<Struct::StaticModelRecord>& a_staticModelRecord) const;

	private:

		DescriptorPool<SRVDescriptorHeap>& m_srvDescriptorPool;
	};
}