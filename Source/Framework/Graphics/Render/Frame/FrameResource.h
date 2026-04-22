#pragma once

namespace FWK::Graphics
{
	class FrameResource final
	{
	public:

		 FrameResource() = default;
		~FrameResource() = default;

		bool Create(const Device& a_device);

		const auto& GetREFDirectCommandAllocator() const { return m_directCommandAllocator; }

		auto& GetMutableREFDirectCommandAllocator() { return m_directCommandAllocator; }

	private:

		DirectCommandAllocator m_directCommandAllocator = {};
	};
}