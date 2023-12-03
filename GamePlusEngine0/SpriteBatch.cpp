
#include "SpriteBatch.h"
#include "Defines.h"
#include "Color.h"


namespace IceEngine
{
	const std::string vertex_shader = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;

		uniform mat4 projection;
		uniform mat4 view;
		uniform mat4 model;
    
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
		}
	)";

	const std::string fragment_shader = R"(
		#version 330 core
		out vec4 FragColor;

		void main()
		{
			FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		}
	)";

	void SpriteBatch::Begin()
	{
		// Setup Shader
		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		// Setup Vertex Buffer

		// Set up Index Buffer

		// Set up vertices for a quad with positions, colors, and texture coordinates
		std::vector<float> vertices = {
			 +0.5f, +0.5f, 0.0f,  // top right
			 +0.5f, -0.5f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  // bottom left
			-0.5f, +0.5f, 0.0f    // top left
		};


		// Set up indices for a quad
		std::vector<unsigned int> indices = {
			0, 1, 3, // first triangle
			1, 2, 3 // second triangle
		};

		// Assuming you have VertexBufferLayout and VertexBuffer classes
		VertexBufferLayout vertexLayout;
		vertexLayout.Push<float>(3);  // Position
		
		m_vertexArray.Bind();

		m_vertexBuffer.SetData(vertices.data(), vertices.size() * sizeof(float));

		m_vertexArray.AddBuffer(m_vertexBuffer, vertexLayout);

		m_indexBuffer.SetData(indices.data(), indices.size());

		// Setup Camera
		m_cameraComponent = new  IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		m_transformComponent = new IceEngine::TransformComponent(glm::vec2(100,100), glm::vec2(100, 100), 0.0f);
	}

	void SpriteBatch::Render()
	{
		Color::SetClearColor({ 1, 71, 94 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		// Setup Camera View Transform

		// Render Vertex Array 

		m_shader->Bind();

		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

		m_vertexArray.Bind();

		m_transformComponent->position = glm::vec2(100, 100);
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);

		m_transformComponent->position = glm::vec2(400, 400);
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix() );

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);
		
		// Unbind buffers
		m_vertexArray.Unbind();
	
	}

	void SpriteBatch::End()
	{
		 
	}
}