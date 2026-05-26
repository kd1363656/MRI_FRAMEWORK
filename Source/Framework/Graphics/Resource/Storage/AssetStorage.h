#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedAssetRecordBaseConcept RecordType>
	class AssetStorage final
	{
	private:

		using RecordMap = std::unordered_map<std::wstring, std::shared_ptr<RecordType>>;

	public:

		 AssetStorage() = default;
		~AssetStorage() = default;

		void Deserialize(const nlohmann::json& a_rootJson)
		{
			if (a_rootJson.is_null()) { return; }
			m_assetStorageJsonConverter.Deserialize(a_rootJson, *this);
		}
		bool Create()
		{
			if (!m_storageIDAllocator.Create())
			{
				assert(false && "StorageIDAllocatorの作成に失敗したため、AssetStorageの作成に失敗しました。");
				return false;
			}

			return true;
		}

		nlohmann::json Serialize() const
		{
			return m_assetStorageJsonConverter.Serialize(*this);
		}

		TypeAlias::StorageID AllocateStorageID()
		{
			return m_storageIDAllocator.Allocate();
		}

		void ReleaseStorageID(const TypeAlias::StorageID a_storageID)
		{
			m_storageIDAllocator.Release(a_storageID);
		}

		bool RegisterRecord(const std::wstring& a_filePath, const std::shared_ptr<RecordType>& a_record)
		{
			if (!a_record)
			{
				assert(false && "レコードのインスタンス化がされていません。");
				return false;
			}

			if (a_filePath.empty())
			{
				assert(false && "ファイルパスが空のため、Recordの登録に失敗しました。");
				return false;
			}

			if (a_record->m_storageID == Constant::k_invalidStorageID)
			{
				assert(false && "StorageIDが無効のため、Recordの登録に失敗しました。");
				return false;
			}

			if (m_recordMap.contains(a_filePath))
			{
				assert(false && "同じファイルパスのRecordが既に登録されているため、Recordの登録に失敗しました。");
				return false;
			}

			m_recordMap.try_emplace(a_filePath, a_record);

			return true;
		}

		bool UnregisterRecord(const std::weak_ptr<RecordType>& a_record)
		{
			const auto& l_record = a_record.lock();

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、Recordの登録解除に失敗しました。");
				return false;
			}

			// ストレージIDを解放し、レコード情報を削除
			m_storageIDAllocator.Release(l_record->m_storageID);
			m_recordMap.erase			(l_record->m_filePath);

			return true;
		}

		bool AddReference(const std::weak_ptr<RecordType>& a_record)
		{
			const auto& l_record = a_record.lock();

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、参照数加算に失敗しました。");
				return false;
			}

			// 参照カウントを加算
			++l_record->m_referenceCount;

			return true;
		}

		bool ReleaseReference(const DirectCommandQueue& a_directCommandQueue, const std::weak_ptr<RecordType>& a_record)
		{
			const auto& l_record = a_record.lock();

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、解放予約に失敗しました。");
				return false;
			}

			if (l_record->m_referenceCount == Constant::k_emptyAssetReferenceCount)
			{
				assert(false && "参照数が0のRecordに対してさらに解放要求が行われました。");
				return false;
			}

			--l_record->m_referenceCount;

			// まだ利用者が残っているなら何もしない
			if (l_record->m_referenceCount > Constant::k_emptyAssetReferenceCount) { return true; }

			const auto& l_lastSignaledFenceValue = a_directCommandQueue.FetchREFLastSignaledFenceValue();

			// GPUに対して発行されたフェンス値を格納する
			// GPUのフェンス値がこの格納されたフェンス値を超えていたら安全に解放できるということ
			l_record->m_retiredFenceValue = l_lastSignaledFenceValue;

			return true;
		}

		template <typename RecordReleaserType>
		void ReleaseCompletedUnusedRecords(const DirectCommandQueue& a_directCommandQueue, RecordReleaserType& a_recordReleaser)
		{
			const auto& l_completedFenceValue = a_directCommandQueue.FetchVALCompletedFenceValue();

			auto l_itr = m_recordMap.begin();

			while (l_itr != m_recordMap.end())
			{
				auto& l_record = l_itr->second;

				if (!l_record)
				{
					l_itr = m_recordMap.erase(l_itr);
					continue;
				}

				// まだ参照されているRecordは解放しない
				if (l_record->m_referenceCount > Constant::k_emptyAssetReferenceCount)
				{
					++l_itr;
					continue;
				}

				// 解放予約用のFence値が初期値なら解放しない
				if (l_record->m_retiredFenceValue == Constant::k_unusedFenceValue)
				{
					++l_itr;
					continue;
				}

				// GPUがまだこのRecordを利用している可能性があるため解放しない
				if (l_record->m_retiredFenceValue > l_completedFenceValue)
				{
					++l_itr;
					continue;
				}

				// Record固有のリソースを解放する
				// TextureRecordならTextureResourceやSRVのStorageID返却などを行う
				if (!a_recordReleaser.ReleaseRecord(*l_record))
				{
					assert(false && "Record固有リソースの解放に失敗しました。");
					++l_itr;
					continue;
				}

				// StorageIDを返却する
				m_storageIDAllocator.Release(l_record->m_storageID);
				// ファイルパスから対応するStorageIDを見つけるMapの要素を削除
				l_itr = m_recordMap.erase(l_itr);
			}
		}

		TypeAlias::StorageID FindVALStorageIDFromFilePath(const std::wstring& a_filePath) const
		{
			const auto& l_itr = m_recordMap.find(a_filePath);

			if (l_itr == m_recordMap.end()) { return Constant::k_invalidStorageID; }

			const auto& l_record = l_itr->second;

			if (!l_record) { return Constant::k_invalidStorageID; }

			return l_record->m_storageID;
		}

		std::weak_ptr<RecordType> FindVALRecord(const std::wstring& a_filePath) const
		{
			if (a_filePath.empty())
			{
				assert(false && "ファイルパスが空のため、Recordの取得に失敗しました。");
				return {};
			}

			const auto& l_itr = m_recordMap.find(a_filePath);

			if (l_itr == m_recordMap.end()) { return {}; }

			return l_itr->second;
		}

		const auto& GetREFStorageIDAllocator() const { return m_storageIDAllocator; }

		auto& GetMutableREFStorageIDAllocator() { return m_storageIDAllocator; }

	private:

		RecordMap m_recordMap = {};

		StorageIDAllocator m_storageIDAllocator = {};

		Converter::AssetStorageJsonConverter<RecordType> m_assetStorageJsonConverter = {};
	};
}