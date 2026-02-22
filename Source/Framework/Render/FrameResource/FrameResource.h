#pragma once

namespace FWK::Render
{
	class FrameResource final
	{
	public:

		explicit FrameResource(const GraphicsCommandContext& a_graphicsCommandContext);
		~FrameResource        ();

		void Init  ();
		bool Create();

		const auto& GetGraphicsCommandAllocator() const { return m_graphicsCommandAllocator; }

		UINT64 GetFenceValue() const { return m_fenceValue; }

		void SetFenceValue(const UINT64 a_fenceValue) { m_fenceValue = a_fenceValue; }

	private:

		GraphicsCommandAllocator m_graphicsCommandAllocator;

		UINT64 m_fenceValue;
	};
}