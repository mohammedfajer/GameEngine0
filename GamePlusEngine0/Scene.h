#pragma once
#include "GameObject.h"

namespace IceEngine
{
	class Scene
	{
	public:
		Scene() : m_name("Untitled") {}
		
		Scene(const std::string& sceneName) : m_name(sceneName) {}
		
		void AddGameObject(GameObject* gameObject) { m_gameObjects.push_back(gameObject); }
		
		const std::string& GetName() const { return m_name; }

		void Update(float deltaTime) 
		{
			for (auto& gameObject : m_gameObjects) 
			{
				gameObject->Update(deltaTime);
			}
		}

		~Scene() 
		{
			for (auto& gameObject : m_gameObjects) 
			{
				delete gameObject;
			}	
		}
	protected:
		std::string m_name;
		std::vector<GameObject*> m_gameObjects;
	};
}