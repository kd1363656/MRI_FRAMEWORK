#pragma once

namespace FWK::Graphics
{
	template <typename Type>
	class DrawCommandBase : public IDrawCommand
	{
	public:

		DrawCommandBase() = default;
		~DrawCommandBase() override = default;

		void BeginFrame() override
		{
			m_drawCommandList.clear();
		}

		void RequestDraw(const Type& a_drawCommand)
		{
			m_drawCommandList.emplace_back(a_drawCommand);
		}

		const auto& GetDrawCommandList() const { return m_drawCommandList; }

	private:

		std::vector<Type> m_drawCommandList = {};
	};
}