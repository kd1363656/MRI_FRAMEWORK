#pragma once

namespace FWK::Graphics
{
	template <typename DrawRequestType>
	class DrawRequestList
	{
	public:

		 DrawRequestList() = default;
		~DrawRequestList() = default;

		void BeginFrame()
		{
			// 所有者がもういない参照を保持する必要がない
			std::erase_if(m_drawRequestList, [](const auto& a_drawRequest)
			{
				return a_drawRequest.expired();
			});
		}

		void RegisterDrawRequest(const std::shared_ptr<DrawRequestType>& a_drawRequest)
		{
			FWK_ASSERT_RETURN_IF(!a_drawRequest, "DrawRequestが無効のため、DrawRequestの登録に失敗しました。")

			m_drawRequestList.emplace_back(a_drawRequest);
		}

		const auto& GetREFDrawRequestList() const { return m_drawRequestList; }

	private:

		std::vector<std::weak_ptr<DrawRequestType>> m_drawRequestList = {};
	};
}