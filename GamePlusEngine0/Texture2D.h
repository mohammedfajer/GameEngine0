#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>



namespace IceEngine {
	struct Texture2D {
		uint32_t id;
		int width;
		int height;
		int nrChannels;
	};

	Texture2D load_texture(const std::string& filePath, GLenum format = GL_RGB, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT, GLenum minFilter = GL_LINEAR, GLenum magFilter = GL_LINEAR);

}