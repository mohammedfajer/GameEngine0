#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Logger.h"

namespace IceEngine {

	Texture2D load_texture(const std::string& filePath, GLenum format,
		GLenum wrapS, GLenum wrapT, GLenum minFilter, GLenum magFilter) {
		IceEngine::Logger& logger = IceEngine::Logger::GetInstance();
		logger.Log("Loading texture " + filePath);


		Texture2D texture;

		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filePath.c_str(), &texture.width, &texture.height, &texture.nrChannels, 0);

		if (data) {
			GLenum format = (texture.nrChannels == 4) ? GL_RGBA : GL_RGB;
			GLenum internalFormat = (format == GL_RGBA) ? GL_RGBA : GL_RGB;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.width,
				texture.height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			

			logger.Log("Failed to load texture from " + filePath, LogLevel::ERROR);
		}

		logger.Log("Successfully loaded texture " + filePath, IceEngine::LogLevel::SUCCESS);

		stbi_image_free(data);
		return texture;
	}
}