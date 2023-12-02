#include "SceneManager.h"
#include "Scene.h"

namespace IceEngine 
{
	SceneManager& SceneManager::Instance()
	{
		static SceneManager instance;
		return instance;
	}

	void SceneManager::AddScene(const std::string& sceneName, Scene* scene)
	{
		m_scenes[sceneName] = scene;
	}

	void SceneManager::SetActiveScene(const std::string& sceneName)
	{
		auto it = m_scenes.find(sceneName);

		if (it != m_scenes.end()) 
		{
			m_currentScene = it->second;
		}
	}

	void SceneManager::UpdateCurrentScene(float deltaTime)
	{
		if (m_currentScene) 
		{
			m_currentScene->Update(deltaTime);
		}
	}

	void SceneManager::RenderCurrentScene()
	{
		if (m_currentScene)
		{
			m_currentScene->Render();
		}
	}

	SceneManager::~SceneManager()
	{
		/*for (auto& pair : m_scenes)
		{
			delete pair.second;
		}*/
	}
}