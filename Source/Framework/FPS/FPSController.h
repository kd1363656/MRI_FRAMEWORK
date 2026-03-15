#pragma once

namespace FWK
{
	class FPSController final
	{
	public:

		FPSController();
		~FPSController();

		void Init();

		void LoadConfig();
		void SaveConfig();

		void BeginUpdate();
		void EndUpdate();

		void SetTargetFPS(const float a_set) { m_targetFPS = a_set; }

		float GetCurrentFPS() const { return m_currentFPS; }
		float GetTargetFPS()  const { return m_targetFPS; }

	private:

		void LimitFramerate() const;
		
		static constexpr float k_defaultTimeScale = 1.0F;
		static constexpr float k_second           = 1.0F;
		static constexpr float k_milliSecond      = 1000.0F;

		const std::filesystem::path k_configFileIOPath = "Asset/Data/Config/FPS/FPSConfig.json";

		Converter::FPSControllerJsonConverter m_fpsControllerJsonConverter;

		std::chrono::steady_clock::time_point m_previousTime;
		std::chrono::steady_clock::time_point m_frameBeginTime;

		float m_timeScale;
		float m_deltaTime;
		float m_scaledDeltaTime;

		float m_currentFPS;
		float m_targetFPS;
	};
}