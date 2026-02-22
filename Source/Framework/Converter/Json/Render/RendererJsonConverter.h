#pragma once

namespace FWK::Render
{
	class Renderer;
}

namespace FWK::Converter
{
	class RendererJsonConverter final : public JsonConverterBase
	{
	public:

		explicit RendererJsonConverter(Render::Renderer& a_renderer);
		~RendererJsonConverter        ()							 override;

		void           Deserialize(const nlohmann::json& a_rootJson) override;
		nlohmann::json Serialize  ()								 override;

	private:

		Render::Renderer& m_renderer;
	};
}