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

			const auto l_descriptorCapacityNum = a_rootJson.value("DescriptorCapacity", Constant::k_defaultDescriptorCapacity);

			a_descriptorPool.SetDescriptorCapacity(l_descriptorCapacityNum);
		}

		nlohmann::json Serialize(const FWK::Graphics::DescriptorPool<Type>& a_descriptorPool) const
		{
			nlohmann::json l_rootJson = {};

			l_rootJson["DescriptorCapacity"] = a_descriptorPool.GetVALDescriptorCapacity();

			return l_rootJson;
		}
	};
}