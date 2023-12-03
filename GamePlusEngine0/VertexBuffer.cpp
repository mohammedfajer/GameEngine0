#include "VertexBuffer.h"
#include <GL/glew.h>

namespace IceEngine
{
	VertexBuffer::VertexBuffer(const void* data, size_t size) 
	{
		SetData(data, size);
	}

	VertexBuffer::~VertexBuffer() 
	{
		glDeleteBuffers(1, &rendererID);
	}

	void VertexBuffer::SetData(const void* data, size_t size)
	{
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	// to be used with the sprite batch
	void VertexBuffer::SetDataDynamics(size_t size)
	{
		glGenBuffers(1, &rendererID);
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	void VertexBuffer::Bind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, rendererID);
	}

	void VertexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}