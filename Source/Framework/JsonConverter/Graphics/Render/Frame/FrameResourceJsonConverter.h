#pragma once

namespace FWK::Graphics
{
	class FrameResource;
}

namespace FWK::JsonConverter
{
	class FrameResourceJsonConverter final
	{
	public:

		 FrameResourceJsonConverter() = default;
		~FrameResourceJsonConverter() = default;

		void Deserialize(const nlohmann::json& a_rootJson, Graphics::FrameResource& a_frameResource) const;

		nlohmann::json Serialize(const Graphics::FrameResource& a_frameResource) const;

	private:

		static constexpr std::string_view k_constantBufferMapJsonKey      = "ConstantBufferMap";
		static constexpr std::string_view k_constantBufferTypeNameJsonKey = "ConstantBufferTypeName";
		static constexpr std::string_view k_constantBufferJsonKey         = "ConstantBuffer";
	};
}