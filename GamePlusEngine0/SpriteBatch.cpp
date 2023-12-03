
#include "SpriteBatch.h"
#include "Defines.h"
#include "Color.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "TextureLoader.h"

namespace IceEngine
{

	const std::string vertex_shader = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec3 aColor;
		layout (location = 2) in vec2 aTexCoord;
		layout (location = 3) in float aTexIdx;

		out vec3 OurColor;
		out vec2 TexCoord;
		out float TexIdx;

		uniform mat4 projection;
		uniform mat4 view;
		uniform mat4 model;
    
		void main()
		{
			gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
			OurColor = aColor;
			TexCoord = aTexCoord;
			TexIdx = aTexIdx;
		}
	)";

	const std::string fragment_shader = R"(
		#version 330 core
		out vec4 FragColor;

		in vec3 OurColor;
		in vec2 TexCoord;
		in float TexIdx;

		uniform sampler2D u_Textures[2];

		void main()
		{
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord);
			
		}
	)";

	void SpriteBatch::Begin()
	{
		// Load Texture 1
		Texture2D coinTexture   = IceEngine::TextureLoader::LoadTexture("./data/coin.png");
		m_coinTextureId = coinTexture.id;

		// Load Texture 2
		Texture2D playerTexture = IceEngine::TextureLoader::LoadTexture("./data/player.png");
		m_playerTextureId = playerTexture.id;

		// Setup Shader
		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());

		m_shader->Bind();
		auto loc = glGetUniformLocation(m_shader->id, "u_Textures");
		int samplers[2] = { 0, 1 };
		glUniform1iv(loc, 2, samplers);


		// Setup Vertex Buffer

		// Set up Index Buffer

		// Set up vertices for a quad with positions, colors, and texture coordinates

		std::vector<float> vertices = {
			// Position					// Color				// Texture Coordinate			// Texture Index
			-1.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 0.0f,						0.0f,
			-0.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 0.0f,						0.0f,
			-0.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 1.0f,						0.0f,
			-1.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 1.0f,						0.0f,

			 0.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 0.0f,						1.0f,
			 1.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 0.0f,						1.0f,
			 1.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 1.0f,						1.0f,
			 0.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 1.0f,						1.0f
		};










		// Set up indices for a quad
		std::vector<unsigned int> indices = {
			0, 1, 3, // first triangle
			1, 2, 3, // second triangle

			4, 5, 6,
			6, 7, 4
		};

		// Assuming you have VertexBufferLayout and VertexBuffer classes
		VertexBufferLayout vertexLayout;
		vertexLayout.Push<float>(3);  // Position
		vertexLayout.Push<float>(3);  // Color
		vertexLayout.Push<float>(2);  // Texture Coordinate
		vertexLayout.Push<float>(1);  // Texture Index

		m_vertexArray.Bind();

		m_vertexBuffer.SetData(vertices.data(), vertices.size() * sizeof(float));

		m_vertexArray.AddBuffer(m_vertexBuffer, vertexLayout);

		m_indexBuffer.SetData(indices.data(), indices.size());

		// Setup Camera
		m_cameraComponent = new  IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		m_transformComponent = new IceEngine::TransformComponent(glm::vec2(0,0), glm::vec2(100, 100), 0.0f);

		m_cameraComponent->SetFollowPosition(m_transformComponent->position, glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	void SpriteBatch::Render()
	{
		Color::SetClearColor({ 1, 71, 94 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		// Setup Camera View Transform

		// Render Vertex Array 

		m_shader->Bind();
		glBindTextureUnit(0, m_coinTextureId);
		glBindTextureUnit(1, m_playerTextureId);

		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

		m_vertexArray.Bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);
		
		// Unbind buffers
		m_vertexArray.Unbind();
	}

	void SpriteBatch::End()
	{
		 
	}
}