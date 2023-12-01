#pragma once

#include <GL/glew.h>

namespace IceEngine 
{
	struct Texture2D
	{
		int width;
		int height;
		int nrChannels;
		GLuint id;
	};
}