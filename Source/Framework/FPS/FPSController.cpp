#include "FPSController.h"

FWK::FPSController::FPSController() :
	m_fpsControllerJsonConverter(*this),
	m_previousTime				(std::chrono::steady_clock::now()),
	m_frameBeginTime            (m_previousTime),
	m_deltaTime                 (0.0F),
	m_timeScale                 (k_defaultTimeScale),
	m_scaledDeltaTime           (0.0F),
	m_nowFPS					(0.0F),
	m_targetFPS					(CommonConstant::k_defaultFPS)
{}
FWK::FPSController::~FPSController() = default;

void FWK::FPSController::Init()
{
	// 過去の時間として現在の時間を格納
	m_previousTime = std::chrono::steady_clock::now();

	// モニタリング用の時刻も現在の時間を格納
	m_frameBeginTime = m_previousTime;

	m_deltaTime       = 0.0F;
	m_timeScale       = k_defaultTimeScale;
	m_scaledDeltaTime = 0.0F;

	m_targetFPS = CommonConstant::k_defaultFPS;
	m_nowFPS    = 0.0F;
}

void FWK::FPSController::LoadCONFIG()
{
	const auto& l_rootJson = Utility::FileIO::LoadJsonFile(k_configFileIOPath);

	m_fpsControllerJsonConverter.Deserialize(l_rootJson);
}
void FWK::FPSController::SaveCONFIG()
{
	const auto& l_rootJson = m_fpsControllerJsonConverter.Serialize();

	Utility::FileIO::SaveJsonFile(l_rootJson, k_configFileIOPath);
}

void FWK::FPSController::UpdateStartTime()
{
	// 現在のフレームでの時間
	m_frameBeginTime = std::chrono::steady_clock::now();

	// デルタタイム(一フレーム前の時間と現フレームでの時間を引く)
	std::chrono::duration<float> l_elapsed = m_frameBeginTime - m_previousTime;

	// 秒単位で取得(ミリ秒に取得する必要がある)
	m_deltaTime = l_elapsed.count();

	// TimeScaleの影響を受けるデルタタイムを計算
	m_scaledDeltaTime = m_deltaTime * m_timeScale;
}
void FWK::FPSController::Update()
{
	// フレームレートを一定に保つ
	LimitFrameRate();

	// FPSを計測する
	MonitorFPS();

	// 過去の経過時間として現在の時刻を入れる
	m_previousTime = m_frameBeginTime;
}

void FWK::FPSController::LimitFrameRate() const
{
	// 現在の時刻を取得
	const auto& l_currentTime = std::chrono::steady_clock::now();

	// 1000ミリ秒 / 目標FPSで今回使用すべき時間を算出
	const auto l_frameTime = std::chrono::milliseconds(static_cast<int>(k_milliSecond / m_targetFPS));

	// 現在の時間を過去の時間と引いてやることで経過時間を算出
	const auto l_elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(l_currentTime - m_frameBeginTime);

	// 使用しきれていない、今回の処理が速く想定のFPSを超えてしまわないようにSleep
	if (l_elapsedTime < l_frameTime)
	{
		std::this_thread::sleep_for(l_frameTime - l_elapsedTime);
	}
}
void FWK::FPSController::MonitorFPS()
{
	if (m_deltaTime > 0.0F)
	{
		// FPS = 1.0F(秒) / DeltaTime(ミリ秒)
		m_nowFPS = k_second / m_deltaTime;
	}
	else
	{
		m_nowFPS = m_targetFPS;
	}
}