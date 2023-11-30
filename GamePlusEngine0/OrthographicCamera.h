#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace IceEngine {
	struct OrthographicCamera {
		glm::mat4 projection;
		glm::vec2 position;
		float zoom;

		OrthographicCamera(float left, float right, float bottom, float top, float zoomLevel = 1.0f)
			: position(0.0f, 0.0f), zoom(zoomLevel) {
			set_projection(left, right, bottom, top);
		}

		void set_projection(float left, float right, float bottom, float top) {
			projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		}

		glm::mat4 get_view_matrix() const {
			auto R = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, -position.y, 1.0)) *
				glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
			return (R);
		}
	};

}