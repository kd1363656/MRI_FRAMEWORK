#pragma once

namespace FWK::Render
{
	class SwapChain;
}

namespace FWK::Converter
{
	class SwapChainJsonConverter final : public JsonConverterBase
	{
	public:

		explicit SwapChainJsonConverter(Render::SwapChain& a_swapChain);
		~SwapChainJsonConverter        ()							    override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()                                 override;

	private:

		Render::SwapChain& m_swapChain;
	};
}