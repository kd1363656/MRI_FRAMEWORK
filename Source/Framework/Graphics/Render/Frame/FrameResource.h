#pragma once

namespace FWK::Graphics
{
	class FrameResource final
	{
	public:

		 FrameResource() = default;
		~FrameResource() = default;

		bool Create(const Device& a_device);

	private:

		DirectCommandAllocator m_directCommandAllocator = DirectCommandAllocator();
	};
}