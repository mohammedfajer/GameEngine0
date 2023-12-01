#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.h"


namespace IceEngine
{
	struct OrthographicCameraComponent : public Component
	{
		glm::mat4 projection;
		glm::vec2 position;
		float zoom;

		OrthographicCameraComponent(float left, float right, float bottom, float top, float zoomLevel = 1.0f) : position(0.0f, 0.0f), zoom(zoomLevel) 
		{
			SetProjection(left, right, bottom, top);
		}

		void SetProjection(float left, float right, float bottom, float top) 
		{
			projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		}

		glm::mat4 GetViewMatrix() const 
		{
			auto Result = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 1.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
			return (Result);
		}

		void Update(float deltaTime) override 
		{
			
		}
	};
}