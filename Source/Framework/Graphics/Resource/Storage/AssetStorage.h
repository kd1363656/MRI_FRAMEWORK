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

		bool RegisterRecord(const std::shared_ptr<RecordType>& a_record, const std::wstring& a_filePath)
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

			if (a_record->GetVALStorageID() == Constant::k_invalidStorageID)
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

		bool AddReference(const std::weak_ptr<RecordType>& a_record)
		{
			const auto& l_record = a_record.lock();

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、参照数加算に失敗しました。");
				return false;
			}

			// 参照カウントを加算
			l_record->AddReferenceCount();

			return true;
		}

		bool ReleaseReference(const std::weak_ptr<RecordType>& a_record, const DirectCommandQueue& a_directCommandQueue, DeferredResourceReleaseQueue& a_deferredResourceReleaseQueue)
		{
			const auto& l_record = a_record.lock();

			if (!l_record)
			{
				assert(false && "指定されたStorageIDのRecordが見つからないため、解放予約に失敗しました。");
				return false;
			}

			if (!l_record->SubtractReferenceCount())
			{
				assert(false && "Recordの参照数減算に失敗しました。");
				return false;
			}
			
			// まだ利用者が残っているなら何もしない
			if (!l_record->IsUnused()) { return true; }

			// Record自身に、GPUResourceやDescriptorIndexを遅延解放Queueへ積ませる
			if (const auto& l_lastSignaledFenceValue = a_directCommandQueue.FetchREFLastSignaledFenceValue();
				!l_record->PushDeferredRelease(l_lastSignaledFenceValue, a_deferredResourceReleaseQueue))
			{
				assert(false && "Record固有リソースの遅延解放Queue登録に失敗しました。");
				return false;
			}

			const auto& l_filePath  = l_record->GetREFFilePath ();
			const auto  l_storageID = l_record->GetVALStorageID();

			if (l_storageID != Constant::k_invalidStorageID)
			{
				m_storageIDAllocator.Release(l_storageID);
			}

			l_record->InvalidateStorageID();

			m_recordMap.erase(l_filePath);

			return true;
		}

		TypeAlias::StorageID FindVALStorageIDFromFilePath(const std::wstring& a_filePath) const
		{
			const auto& l_itr = m_recordMap.find(a_filePath);

			if (l_itr == m_recordMap.end()) { return Constant::k_invalidStorageID; }

			const auto& l_record = l_itr->second;

			if (!l_record) { return Constant::k_invalidStorageID; }

			return l_record->GetVALStorageID();
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