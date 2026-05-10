#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedAssetRecordBaseConcept RecordType>
	class AssetStorage final
	{
	private:

		using FilePathStorageIDMap = std::unordered_map<std::wstring,		  TypeAlias::StorageID, Struct::WStringHash, std::equal_to<>>;
		using RecordMap            = std::unordered_map<TypeAlias::StorageID, RecordType>;

	public:

		 AssetStorage() = default;
		~AssetStorage() = default;

		bool Create(const TypeAlias::StorageID a_storageIDCapacity)
		{
			if (!m_storageIDAllocator.Create(a_storageIDCapacity))
			{
				assert(false && "StorageIDAllocatorの作成に失敗したため、AssetStorageの作成に失敗しました。");
				return false;
			}

			return true;
		}

		TypeAlias::StorageID Allocate()
		{
			return m_storageIDAllocator.Allocate();
		}

		void Release(const TypeAlias::StorageID a_storageID)
		{
			m_storageIDAllocator.Release(a_storageID);
		}

		bool RegisterRecord(const std::wstring& a_filePath, const RecordType& a_record)
		{
			if (a_filePath.empty())
			{
				assert(false && "ファイルパスが空のため、Recordの登録に失敗しました。");
				return false;
			}

			if (a_record.m_storageID == Constant::k_invalidStorageID)
			{
				assert(false && "StorageIDが無効のため、Recordの登録に失敗しました。");
				return false;
			}

			if (m_filePathStorageIDMap.contains(a_filePath))
			{
				assert(false && "同じファイルパスのRecordが既に登録されているため、Recordの登録に失敗しました。");
				return false;
			}

			if (m_recordMap.contains(a_record.m_storageID))
			{
				assert(false && "同じStorageIDのRecordが既に登録されているため、Recordの登録に失敗しました。");
				return false;
			}

			m_filePathStorageIDMap.try_emplace(a_filePath,			 a_record.m_storageID);
			m_recordMap.try_emplace			  (a_record.m_storageID, a_record);

			return true;
		}

		bool UnregisterRecord(const TypeAlias::StorageID a_storageID)
		{
			auto* l_record = FindMutablePTRRecord(a_storageID);

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、Recordの登録解除に失敗しました。");
				return false;
			}

			const auto l_filePath = l_record->m_filePath;

			m_filePathStorageIDMap.erase(l_filePath);
			m_recordMap.erase			(a_storageID);

			m_storageIDAllocator.Release(a_storageID);

			return true;
		}

		bool AddReference(const TypeAlias::StorageID a_storageID)
		{
			auto* l_record = FindMutablePTRRecord(a_storageID);

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、参照数加算に失敗しました。");
				return false;
			}

			// 参照カウントを加算
			++l_record->m_referenceCount;

			return true;
		}

		bool ReleaseReference(const DirectCommandQueue& a_directCommandQueue, const TypeAlias::StorageID a_storageID)
		{
			auto* l_record = FindMutablePTRRecord(a_storageID);

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

				// まだ参照されているRecordは解放しない
				if (l_record.m_referenceCount > Constant::k_emptyAssetReferenceCount)
				{
					++l_itr;
					continue;
				}

				// 解放予約用のFence値が初期値なら解放しない
				if (l_record.m_retiredFenceValue == Constant::k_unusedFenceValue)
				{
					++l_itr;
					continue;
				}

				// GPUがまだこのRecordを利用している可能性があるため解放しない
				if (l_completedFenceValue < l_record.m_retiredFenceValue)
				{
					++l_itr;
					continue;
				}

				if (!a_recordReleaser.ReleaseRecord(l_record))
				{
					assert(false && "Record固有リソースの解放に失敗しました。");
					++l_itr;
					continue;
				}

				// ファイルパスから対応するStorageIDを見つけるMapの要素を削除
				m_filePathStorageIDMap.erase(l_record.m_filePath);

				// StorageIDを返却する
				m_storageIDAllocator.Release(l_record.m_storageID);

				// RecordMapから削除する
				// erase()は削除した次のイテレーターを返す
				l_itr = m_recordMap.erase(l_itr);
			}
		}

		TypeAlias::StorageID FindVALStorageIDFromFilePath(const std::wstring& a_filePath) const
		{
			const auto& l_itr = m_filePathStorageIDMap.find(a_filePath);

			if (l_itr == m_filePathStorageIDMap.end()) { return Constant::k_invalidStorageID; }

			return l_itr->second;
		}

		const RecordType* FindPTRRecord(const TypeAlias::StorageID a_storageID) const
		{
			if (a_storageID == Constant::k_invalidStorageID)
			{
				assert(false && "StorageIDが無効のため、Recordの取得に失敗しました。");
				return nullptr;
			}

			const auto& l_itr = m_recordMap.find(a_storageID);

			if (l_itr == m_recordMap.end())
			{
				assert(false && "指定されたStorageIDに対応するRecordが見つかりませんでした。");
				return nullptr;
			}

			return &l_itr->second;
		}

		RecordType* FindMutablePTRRecord(const TypeAlias::StorageID a_storageID)
		{
			if (a_storageID == Constant::k_invalidStorageID)
			{
				assert(false && "StorageIDが無効のため、Recordの取得に失敗しました。");
				return nullptr;
			}

			const auto& l_itr = m_recordMap.find(a_storageID);

			if (l_itr == m_recordMap.end())
			{
				assert(false && "指定されたStorageIDに対応するRecordが見つかりませんでした。");
				return nullptr;
			}

			return &l_itr->second;
		}

	private:

		FilePathStorageIDMap m_filePathStorageIDMap = {};
		RecordMap			 m_recordMap			= {};

		StorageIDAllocator m_storageIDAllocator = {};
	};
}