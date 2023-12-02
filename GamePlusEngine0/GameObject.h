#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "Component.h"

namespace IceEngine 
{
	class GameObject 
	{
	public:
		template<typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			std::unique_ptr<T> newComponent = std::make_unique<T>(std::forward<Args>(args)...);
			m_components[typeid(T).name()] = std::move(newComponent);

			return dynamic_cast<T*>(m_components[typeid(T).name()].get());
		}

		template <typename T>
		T* GetComponent() const 
		{
			auto it = m_components.find(typeid(T).name());
			if (it != m_components.end()) 
			{
				Component* baseComponent = it->second.get();
				// Use dynamic_cast to check if the cast is valid
				return dynamic_cast<T*>(baseComponent);
			}
			return nullptr;
		}

		void Update(float deltaTime) 
		{
			for (auto& pair : m_components) 
			{
				pair.second->Update(deltaTime);
			}
		}

		void Render()
		{

		}

		~GameObject() = default;

		void SetName(std::string name) { m_name = name; }
	private:
		std::string m_name;
		std::string m_tag;
		std::string m_layer;
		std::unordered_map<std::string, std::unique_ptr<Component>> m_components;
		std::vector<std::unique_ptr<GameObject>> m_children;
	};

}