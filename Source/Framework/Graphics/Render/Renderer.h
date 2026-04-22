#pragma once

namespace FWK::Graphics
{
	class Renderer final
	{
	public:

		 Renderer() = default;
		~Renderer() = default;

		void Deserialize(const nlohmann::json& a_rootJson);
		bool Create     (const Device&		   a_device);

		nlohmann::json Serialize() const;

		void AddFrameResource(const FrameResource& a_frameResource);

		const auto& GetREFResourceList() const { return m_frameResourceList; }

	private:

		DirectCommandQueue m_directCommandQueue = {};
		DirectCommandList  m_directCommandList  = {};
		
		Converter::RendererJsonConverter m_rendererJsonConverter = {};

		std::vector<FrameResource> m_frameResourceList = {};
	};
}