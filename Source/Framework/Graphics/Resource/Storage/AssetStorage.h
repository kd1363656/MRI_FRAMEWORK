#pragma once

namespace FWK::Graphics
{
	template <typename Type>
	class AssetStorage final
	{
	private:

		using FilePathStorageIDMap = std::unordered_map<std::wstring,		  TypeAlias::StorageID, Struct::WStringHash, std::equal_to<>>;
		using RecordMap            = std::unordered_map<TypeAlias::StorageID, Type>;

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

		bool ContainsFilePath(const std::wstring& a_filePath) const 
		{
			return m_filePathStorageIDMap.contains(a_filePath);
		}

		TypeAlias::StorageID FindVALStorageIDFromFilePath(const std::wstring& a_filePath)
		{
			const auto& l_itr = m_filePathStorageIDMap.find(a_filePath);

			if (l_itr == m_filePathStorageIDMap.end()) { return Constant::k_invalidStorageID; }

			return l_itr->second;
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
				assert(false && "参照数が0のrecordに対してさらに解放要求が行われました。");
				return false;
			}

			--l_record->m_referenceCount;

			// 参照カウントが0以上ならreturn
			if (l_record->m_refereneceCount > Constant::k_emptyAssetReferenceCount) { return true; }

			const auto& l_lastSignaledFenceValue = a_directCommandQueue.FetchREFLastSignaledFenceValue();

			// GPUに対して発行されたフェンス値を格納する
			// GPUのフェンス値がこの書くのされたフェンス値を超えていたら安全に開放できるということ
			l_record->m_referenceFenceValue = l_lastSignaledFenceValue;

			return true;
		}

		const Type* FindPTRRecord(const TypeAlias::StorageID a_storageID) const
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

		const Type* FindMutablePTRRecord(const TypeAlias::StorageID a_storageID) const
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