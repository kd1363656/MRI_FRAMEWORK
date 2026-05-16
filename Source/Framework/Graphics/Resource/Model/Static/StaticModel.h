#pragma once

namespace FWK::Graphics
{
	class StaticModel final
	{
	public:

		 StaticModel();
		 StaticModel(const StaticModel&  a_other);
		 StaticModel(      StaticModel&& a_other) noexcept;
		~StaticModel();

		StaticModel& operator=(const StaticModel&  a_other);
		StaticModel& operator=(	     StaticModel&& a_other) noexcept;

		bool Load(const std::filesystem::path& a_filePath);

		bool IsValid() const;

		const auto& GetREFStaticModelRecord() const { return m_staticModelRecord; }
		
		auto GetVALStorageID() const { return m_storageID; }

	private:

		void AddStaticModelReference() const;

		void ReleaseStaticModelReference();

		TypeAlias::StorageID m_storageID;

		std::weak_ptr<Struct::StaticModelRecord> m_staticModelRecord;
	};
}