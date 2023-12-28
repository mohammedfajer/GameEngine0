#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace IceEngine {

	struct TransformComponent : public Component {
		
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
        
		TransformComponent() : position(0.0f), scale(1.0f), rotation(0.0f) {}

		TransformComponent(glm::vec2 position, glm::vec2 scale, float rotation = 0.0f) :
			position(position), scale(scale), rotation(rotation) {}

		glm::mat4 GetModelMatrix() const {
			// Translation matrix with the pivot adjustment
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(position.x - 0.5f * scale.x, position.y - 0.5f * scale.y, 0.0f));

			// Rotation matrix (assuming rotation is in radians)
			glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

			// Scale matrix
			glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

			// Combine the transformations
			return translation * rotationMatrix * scaling;
		}
		
		void Update(float deltaTime) override { }
	};
}

