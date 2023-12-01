#pragma once

#include <vector>
#include "Logger.h"
#include "GameObject.h"
#include "Scene.h"

namespace IceEngine 
{
	class SceneManager 
	{
	public:
		static SceneManager& Instance();
		void AddScene(const std::string& sceneName, Scene* scene);
		void SetActiveScene(const std::string& sceneName);
		void UpdateCurrentScene(float deltaTime);
	public:
		// Disable copy and move operations
		SceneManager(SceneManager const&) = delete;
		SceneManager(SceneManager&&) = delete;
		SceneManager& operator=(SceneManager const&) = delete;
		SceneManager& operator=(SceneManager&&) = delete;
	private:
		std::unordered_map<std::string, Scene*> m_scenes;
		Scene* m_currentScene = nullptr;
		~SceneManager();
		SceneManager() = default;
	};
}