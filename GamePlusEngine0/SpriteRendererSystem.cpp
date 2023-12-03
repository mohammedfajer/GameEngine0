#include "SpriteRendererSystem.h"

#include "TransformComponent.h"

#include "SpriteRendererComponent.h"

#include "OrthographicCameraComponent.h"

#include "VertexBuffer.h"

namespace IceEngine
{
	SpriteRendererSystem::SpriteRendererSystem(const std::vector<glm::vec2>& textureCoords) : m_shader(nullptr), m_textureCoords(textureCoords)
	{
		// Set up vertices for a quad with positions, colors, and texture coordinates
		std::vector<Vertex> vertices = {
			{ { +0.5f, +0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, m_textureCoords[0] },  // top right
			{ { +0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, m_textureCoords[1] },  // bottom right
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, m_textureCoords[2] },  // bottom left
			{ { -0.5f, +0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, m_textureCoords[3] }   // top left
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
		// TODO(mo): Fix error of the shader texture when drawing game objects that have not textures


		TransformComponent* transformComponent = gameObject.GetComponent<TransformComponent>();
		SpriteRendererComponent* spriteComponent = gameObject.GetComponent<SpriteRendererComponent>();
		OrthographicCameraComponent* cameraComponent = gameObject.GetComponent<OrthographicCameraComponent>();

		m_shader->Bind();

		m_vertexArray.Bind();
		m_indexBuffer.Bind();
		m_vertexBuffer.Bind();

		if (transformComponent)
		{
			m_shader->SetMat4("model", transformComponent->GetModelMatrix());

			if (spriteComponent && spriteComponent->m_texture.id != 0)
			{
				// Only bind the texture if it's valid
				glBindTexture(GL_TEXTURE_2D, spriteComponent->m_texture.id);
			}

			// Draw the sprite
			// You'll need to customize this based on your rendering setup
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);
		}

		if (transformComponent && cameraComponent)
		{
			m_shader->SetMat4("model", transformComponent->GetModelMatrix());

			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);
		}

		// Unbind buffers
		m_vertexArray.Unbind();
		m_indexBuffer.Unbind();
		m_vertexBuffer.Unbind();

		// Update Texture
		if (spriteComponent && spriteComponent->m_texture.id != 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}