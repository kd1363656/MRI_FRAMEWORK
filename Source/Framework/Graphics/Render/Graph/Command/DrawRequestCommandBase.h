#pragma once

namespace FWK::Graphics
{
	// PassConstantあり版
	template <typename ObjectType, typename PassConstantType = void>
	class DrawRequestCommandBase : public DrawCommandBase
	{
	public:

		 DrawRequestCommandBase()		   = default;
		~DrawRequestCommandBase() override = default;

		void BeginFrame() override
		{
			// 所有者がもういない参照を保持する必要がない
			std::erase_if(m_drawCommandList, [](const auto& a_drawCommand)
			{
				return a_drawCommand.expired();
			});
		}

		void RegisterDrawCommand(const std::shared_ptr<ObjectType>& a_drawCommand)
		{
			FWK_ASSERT_RETURN_IF(!a_drawCommand, "DrawCommandが無効のため、DrawCommandの登録に失敗しました。")

			m_drawCommandList.emplace_back(a_drawCommand);
		}
	
		void RegisterPassConstant(const std::shared_ptr<PassConstantType>& a_passConstant)
		{
			FWK_ASSERT_RETURN_IF(!a_passConstant, "PassConstantが無効のため、PassConstantの登録に失敗しました。")

			m_passConstant = a_passConstant;
		}

		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

		const auto& GetPassConstant() const { return m_passConstant; }

	private:

		std::vector<std::weak_ptr<ObjectType>> m_drawCommandList = {};
		std::weak_ptr<PassConstantType>		   m_passConstant	 = {};
	};

	// PassConstantなし版
	template <typename ObjectType>
	class DrawRequestCommandBase<ObjectType, void> : public DrawCommandBase
	{
	public:

		 DrawRequestCommandBase()		   = default;
		~DrawRequestCommandBase() override = default;

		void BeginFrame() override
		{
			// 所有者がもういない参照を保持する必要がない
			std::erase_if(m_drawCommandList, [](const auto& a_drawCommand)
			{
				return a_drawCommand.expired();
			});
		}

		void RegisterDrawCommand(const std::shared_ptr<ObjectType>& a_drawCommand)
		{
			FWK_ASSERT_RETURN_IF(!a_drawCommand, "DrawCommandが無効のため、DrawCommandの登録に失敗しました。")

			m_drawCommandList.emplace_back(a_drawCommand);
		}
	
		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

	private:

		std::vector<std::weak_ptr<ObjectType>> m_drawCommandList = {};
	};
}