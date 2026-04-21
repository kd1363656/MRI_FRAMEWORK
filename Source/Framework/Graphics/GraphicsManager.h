#pragma once

namespace FWK::Graphics
{
	class GraphicsManager final : public SingletonBase<GraphicsManager>
	{
	public:

		void Init  () const;
		bool Create() const;
		
	private:

#if defined(_DEBUG)
		bool EnableDebugLayer() const;
#endif

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<GraphicsManager>;

		 GraphicsManager()          = default;
		~GraphicsManager() override = default;
	};
}