#pragma once

namespace FWK::Graphics
{
	// PassConstantあり版
	template <typename ObjectType, typename PassConstantType = void>
	class DrawRequestCommandBase : public DrawCommandBase
	{
	public:

		 DrawRequestCommandBase()		    = default;
		~DrawRequestCommandBase() override = default;

		void BeginFrame() override
		{
			m_drawCommandList.clear();
			m_passConstant.reset   ();
		}

		void RequestDraw(const ObjectType& a_drawCommand)
		{
			m_drawCommandList.emplace_back(a_drawCommand);
		}

		void SetPassConstant(const PassConstantType& a_passConstant)
		{
			m_passConstant = a_passConstant;
		}

		bool HasPassConstant() const 
		{
			return m_passConstant.has_value();
		}

		const PassConstantType* GetPTRPassConstant() const
		{
			if (!HasPassConstant()) { return nullptr; }
			
			return &m_passConstant.value();
		}
	
		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

	private:

		std::vector<ObjectType>			m_drawCommandList = {};
		std::optional<PassConstantType> m_passConstant	  = std::nullopt;
	};

	// PassConstantなし版
	template <typename ObjectType>
	class DrawRequestCommandBase<ObjectType, void> : public DrawCommandBase
	{
	public:

		 DrawRequestCommandBase()		    = default;
		~DrawRequestCommandBase() override = default;

		void BeginFrame() override
		{
			m_drawCommandList.clear();
		}

		void RequestDraw(const ObjectType& a_drawCommand)
		{
			m_drawCommandList.emplace_back(a_drawCommand);
		}

		const auto& GetREFDrawCommandList() const { return m_drawCommandList; }

	private:

		std::vector<ObjectType> m_drawCommandList = {};
	};
}