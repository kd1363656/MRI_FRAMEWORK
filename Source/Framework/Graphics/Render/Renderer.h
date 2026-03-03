#pragma once

namespace FWK::Graphics
{
	class Renderer
	{
	public:

		explicit Renderer(const Hardware& a_hardware);
		~Renderer        ();

		void Init  ();
		bool Create();

	private:

		static constexpr std::size_t k_frameCount = 3;

		std::vector<FrameResource> m_frameResourceList;

		DirectCommandQueue m_directCommandQueue;
		DirectCommandList  m_directCommandList;

		UINT m_currentFrameIndex = 0U;
	};
}