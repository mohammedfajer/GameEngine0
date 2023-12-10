// TextureLoader.h
#pragma once

#include <string>
#include <GL/glew.h>
#include "Texture2D.h"

#include <unordered_map>

namespace IceEngine {

	class TextureLoader {
	public:
		/*static Texture2D LoadTexture(const std::string& filePath,
			GLenum format = GL_RGBA,
			GLenum wrapS  = GL_REPEAT,
			GLenum wrapT  = GL_REPEAT,
			GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
			GLenum magFilter = GL_LINEAR);*/

		static Texture2D LoadTexture(const std::string &filePath,
			GLenum format = GL_RGBA,
			GLenum wrapS = GL_REPEAT,
			GLenum wrapT = GL_REPEAT,
			GLenum minFilter = GL_LINEAR,
			GLenum magFilter = GL_LINEAR);
	
		static std::unordered_map<std::string, Texture2D> textureCache;
	};

} // namespace IceEngine