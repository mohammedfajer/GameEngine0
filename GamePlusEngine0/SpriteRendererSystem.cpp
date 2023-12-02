#include "SpriteRendererSystem.h"

#include "TransformComponent.h"

#include "SpriteRendererComponent.h"

#include <vector>

#include "VertexBuffer.h"




namespace IceEngine
{
	SpriteRendererSystem::SpriteRendererSystem() : m_shader(nullptr)
	{
		// Set up vertices for a quad with positions, colors, and texture coordinates
		std::vector<Vertex> vertices = {
			{ { +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },  // top right
			{ { +0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },  // bottom right
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  // bottom left
			{ { -0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }   // top left
		};

		// Set up indices for a quad
		std::vector<unsigned int> indices = {
			0, 1, 3, // first triangle
			1, 2, 3 // second triangle
		};

		// Assuming you have VertexBufferLayout and VertexBuffer classes
		VertexBufferLayout vertexLayout;
		vertexLayout.Push<float>(3);  // Position
		vertexLayout.Push<float>(3);  // Color
		vertexLayout.Push<float>(2);  // Texture Coordinates

		m_vertexArray.Bind();

		m_vertexBuffer.SetData(vertices.data(), vertices.size() * sizeof(Vertex));

		m_vertexArray.AddBuffer(m_vertexBuffer, vertexLayout);
		
		m_indexBuffer.SetData(indices.data(), indices.size());
	}

	SpriteRendererSystem::~SpriteRendererSystem()
	{
	}

	void SpriteRendererSystem::SetShader(Shader* shader)
	{
		m_shader = shader;
	}

	void IceEngine::SpriteRendererSystem::RenderGameObject(GameObject& gameObject)
	{
		
		TransformComponent* transformComponent = gameObject.GetComponent<TransformComponent>();
		SpriteRendererComponent* spriteComponent = gameObject.GetComponent<SpriteRendererComponent>();

		if (transformComponent && spriteComponent)
		{
			m_shader->Bind();

			// Bind necessary buffers
			m_vertexArray.Bind();
			m_indexBuffer.Bind();
			m_vertexBuffer.Bind();

			// Set shader uniforms, e.g., sprite position, size, etc.
			m_shader->SetMat4("model", transformComponent->GetModelMatrix());

			glBindTexture(GL_TEXTURE_2D, spriteComponent->m_texture.id);

			// Draw the sprite
			// You'll need to customize this based on your rendering setup
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);

			// Unbind buffers
			m_vertexArray.Unbind();
			m_indexBuffer.Unbind();
			m_vertexBuffer.Unbind();
		}
	}
}