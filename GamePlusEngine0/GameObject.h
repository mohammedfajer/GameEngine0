#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "Component.h"

namespace IceEngine 
{
	class GameObject 
	{
	public:

		template<typename T>
		T* AddComponent() 
		{
			T* newComponent = new T();
			m_components[typeid(T).name()] = newComponent;
			return newComponent;
		}

		
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args) 
		{
			T* newComponent = new T(std::forward<Args>(args)...);
			m_components[typeid(T).name()] = newComponent;
			return newComponent;
		}

		template<typename T>
		T* GetComponent() const 
		{
			auto it = m_components.find(typeid(T).name());
			return it != m_components.end() ? dynamic_cast<T*>(it->second) : nullptr;
		}

		void Update(float deltaTime) 
		{
			for (auto& pair : m_components) 
			{
				pair.second->Update(deltaTime);
			}
		}

		~GameObject() 
		{
			for (auto& pair : m_components) 
			{
				delete pair.second;
			}
		}

		void SetName(std::string name) { m_name = name;  }


	private:
		std::string m_name;
		std::string m_tag;
		std::string m_layer;
		std::unordered_map<std::string, Component*> m_components;
	};

}