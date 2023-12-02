#include "IndexBuffer.h"
#include <GL/glew.h>

namespace IceEngine
{
	IndexBuffer::IndexBuffer(const uint32_t* data, size_t count) : m_count(count)
	{
		SetData(data, count);
	}

	void IndexBuffer::Bind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO_ID);
	}

	void IndexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	IndexBuffer::~IndexBuffer() 
	{
		glDeleteBuffers(1, &m_EBO_ID);
	}
	void IndexBuffer::SetData(const uint32_t* data, size_t count)
	{
		m_count = count;

		glGenBuffers(1, &m_EBO_ID);

		Bind();

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW);
	}
}