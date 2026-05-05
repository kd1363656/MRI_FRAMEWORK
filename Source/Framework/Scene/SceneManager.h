#pragma once

namespace FWK
{
	class SceneManager final : public SingletonBase<SceneManager>
	{
	public:

		void PostLoadSetup();

		void RequestDraw() const;

		void Update();

	private:

		Scene m_scene = {};

		//=========================
		// シングルトン
		//=========================
		friend class SingletonBase<SceneManager>;

		 SceneManager()          = default;
		~SceneManager() override = default;
	};
}