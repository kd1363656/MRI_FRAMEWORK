#pragma once

namespace FWK
{
	class FPSController final
	{
	public:
	
		FPSController ();
		~FPSController();
	
		void Init();

		void LoadCONFIG();
		void SaveCONFIG();
		
		void UpdateStartTime();
		void Update		    ();

		float GetNowFPS   () const { return m_nowFPS; }
		float GetTargetFPS() const { return m_targetFPS; }

		void SetTargetFPS(const float a_set) { m_targetFPS = a_set; }

	private:
	
		void LimitFrameRate() const;
		void MonitorFPS    ();

		static constexpr float k_defaultTimeScale = 1.0F;
		static constexpr float k_second           = 1.0F;
		static constexpr float k_milliSecond      = 1000.0F;

		const std::string k_configFileIOPath = "Asset/Data/CONFIG/FPS/FPSCONFIG.json";
	
		Converter::FPSControllerJsonConverter m_fpsControllerJsonConverter;

		std::chrono::steady_clock::time_point m_previousTime;
		std::chrono::steady_clock::time_point m_frameBeginTime;
	
		float m_deltaTime;
		float m_timeScale;
		float m_scaledDeltaTime;
	
		float m_nowFPS;	
		float m_targetFPS;
	};
}