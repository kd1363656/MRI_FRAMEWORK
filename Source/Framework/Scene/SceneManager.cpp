#include "SceneManager.h"

void FWK::SceneManager::PostLoadSetup()
{
	m_scene.PostLoadSetup();
}

void FWK::SceneManager::RegisterDrawCommand() const
{
	m_scene.RegisterDrawCommand();
}

void FWK::SceneManager::Update()
{
	m_scene.Update();
}