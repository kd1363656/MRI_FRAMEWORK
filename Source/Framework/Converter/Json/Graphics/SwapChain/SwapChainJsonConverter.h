#pragma once

namespace FWK::Graphics
{
	class SwapChain;
}

namespace FWK::Converter
{
	class SwapChainJsonConverter final : public JsonConverterBase
	{
	public:

		explicit SwapChainJsonConverter(Graphics::SwapChain& a_swapChain);
		~SwapChainJsonConverter        ()								  override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()								 override;

	private:

		Graphics::SwapChain& m_swapChain;
	};
}