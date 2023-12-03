#pragma once
#include "GameObject.h"
#include "Logger.h"

namespace IceEngine
{
	class Scene
	{
	public:
		Scene() : m_name("Untitled") {}
		
		Scene(const std::string& sceneName) : m_name(sceneName) {}
		
		void AddGameObject(GameObject* gameObject) { m_gameObjects.push_back(gameObject); }
		
		const std::string& GetName() const { return m_name; }

		// Function to get a GameObject by name
		GameObject* GetGameObjectByName(const std::string& name)
		{
			auto it = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), [&name](const GameObject* obj) 
            { return obj->GetName() == name; });

			if (it != m_gameObjects.end())
			{
				return *it;
			}
			else
			{
				Logger::Instance().Log("GameObject with name " + name + " not found in the scene " + m_name);
				return nullptr; // Return nullptr if not found
			}
		}

		virtual void Update(float deltaTime) 
		{
			for (auto& gameObject : m_gameObjects) 
			{
				gameObject->Update(deltaTime);
			}
		}

		virtual void Render()
		{

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