#include "VertexBufferElement.h"

#include <GL/glew.h>

namespace IceEngine
{
	uint32_t VertexBufferElement::GetSizeOfType(uint32_t type)
	{
		switch (type) 
		{
		case GL_FLOAT:         return sizeof(GLfloat);
		case GL_UNSIGNED_INT:  return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
		default:
			// Handle other types or throw an error
			// For simplicity, assuming all types are handled
			return 0;
		}
	}
}