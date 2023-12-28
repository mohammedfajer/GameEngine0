

// TextureLoader.cpp
#include "TextureLoader.h"
#include "Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace IceEngine {

	std::unordered_map<std::string, Texture2D> TextureLoader::textureCache;

	Texture2D TextureLoader::LoadTexture(const std::string &filePath, GLenum format, GLenum wrapS,
		GLenum wrapT, GLenum minFilter, GLenum magFilter) {
		IceEngine::Logger &logger = IceEngine::Logger::Instance();

		// Check if texture is already in the cache
		auto it = textureCache.find(filePath);
		if (it != textureCache.end()) {
			logger.Log("Using cached texture " + filePath);
			return it->second;
		}

		logger.Log("Loading texture " + filePath);
		logger.Log("");

		// Check for anisotropic filtering support
		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

		// Enable anisotropic filtering with the maximum supported degree
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);

		Texture2D texture;
		glGenTextures(1, &texture.id);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

		// @nocheckin maxFilter was set to GL_LINEAR and when I have changed it to GL_NEAREST it made my texture more clear and remove the blurriness. 
		// https://gamedev.stackexchange.com/questions/19075/how-can-i-make-opengl-textures-scale-without-becoming-blurry

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Enable anisotropic filtering with the maximum supported degree
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);


		stbi_set_flip_vertically_on_load(false);

		unsigned char *data = stbi_load(filePath.c_str(), &texture.width, &texture.height, &texture.nrChannels, 0);
		if (data) {
			GLenum textureFormat = (texture.nrChannels == 4) ? GL_RGBA : GL_RGB;
			GLenum internalFormat = (textureFormat == GL_RGBA) ? GL_SRGB_ALPHA : GL_SRGB;
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.width, texture.height, 0, textureFormat, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			

			// Cache the loaded texture
			textureCache[filePath] = texture;
		}
		else {
			logger.Log("Failed to load texture from " + filePath, LogLevel::ERROR);
		}

		logger.Log("Successfully loaded texture " + filePath, LogLevel::SUCCESS);
		logger.Log("Texture Width = " + std::to_string(texture.width) + " Height = " + std::to_string(texture.height));
		logger.Log("");

		stbi_image_free(data);

		return texture;
	}
}