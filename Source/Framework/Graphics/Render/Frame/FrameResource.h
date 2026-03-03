#pragma once

namespace FWK::Graphics
{
	class FrameResource final
	{
	public:

		explicit FrameResource(const Device& a_device);
		~FrameResource        ();

		void Init  ();
		bool Create();

		const auto& GetDirectCommandAllocator() const { return m_directCommandAllocator; }

		auto& GetWorkDirectCommandAllocator() { return m_directCommandAllocator; }

	private:

		DirectCommandAllocator m_directCommandAllocator;
	};
}