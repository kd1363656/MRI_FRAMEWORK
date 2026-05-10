#pragma once

namespace FWK::Graphics
{
	template <Concept::IsDerivedDescriptorHeapBaseConcept Type>
	class DescriptorPool;
}

namespace FWK::Converter
{
	template <Concept::IsDerivedDescriptorHeapBaseConcept Type>
	class DescriptorPoolJsonConverter final
	{
	public:

		 DescriptorPoolJsonConverter() = default;
		~DescriptorPoolJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, FWK::Graphics::DescriptorPool<Type>& a_descriptorPool) const
		{
			if (a_rootJson.is_null()) { return; }

			const auto l_storageCapacity = a_rootJson.value(k_storageCapacityJsonKey, Constant::k_defaultCreateStorageIDCapacity);

			a_descriptorPool.SetDescriptorCapacity(l_storageCapacity);
		}

		nlohmann::json Serialize(const FWK::Graphics::DescriptorPool<Type>& a_descriptorPool) const
		{
			nlohmann::json l_rootJson = {};

			l_rootJson[k_storageCapacityJsonKey] = a_descriptorPool.GetVALStorageIDCapacity();

			return l_rootJson;
		}

	private:

		static constexpr std::string_view k_storageCapacityJsonKey = "StorageCapacity";
	};
}