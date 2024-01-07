#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.h"
#include "Defines.h"








namespace IceEngine
{
	struct OrthographicCameraComponent : public Component
	{
		glm::mat4 projection;
		glm::vec2 position;
		glm::mat4 view;
		float zoom;

		OrthographicCameraComponent(float left, float right, float bottom, float top, float zoomLevel = 1.0f) : position(0.0f, 0.0f), zoom(zoomLevel) 
		{
			SetProjection(left, right, bottom, top);
			GetViewMatrix();
		}

		void SetProjection(float left, float right, float bottom, float top) 
		{
			projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		}

		void SetFollowPosition(const glm::vec2& playerPosition, const glm::vec2& screenSize)
		{
			position.x = playerPosition.x - screenSize.x / (2.0f * zoom);
			position.y = playerPosition.y - screenSize.y / (2.0f * zoom);
		}

		glm::vec2 worldToScreen(const glm::vec2 &worldCoordinates) const {
			glm::vec4 clipSpace = projection * view * glm::vec4(worldCoordinates.x - position.x, worldCoordinates.y - position.y, 0.0f, 1.0f);

			// Normalize the clip space coordinates
			clipSpace /= clipSpace.w;

			// Convert to screen space
			glm::vec2 screenCoordinates;
			screenCoordinates.x = (clipSpace.x + 1.0f) * 0.5f * SCREEN_WIDTH;
			screenCoordinates.y = (1.0f - clipSpace.y) * 0.5f * SCREEN_HEIGHT;

			return screenCoordinates;
		}

		glm::mat4 GetViewMatrix() const {
			auto view = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 1.0)) * glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
			return (view);
		}

		void Update(float deltaTime) override 
		{
		}
	};
}
