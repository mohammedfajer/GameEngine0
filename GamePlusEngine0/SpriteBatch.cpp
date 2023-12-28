#include "SpriteBatch.h"
#include "Defines.h"
#include "Color.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TextureLoader.h"
#include <array>
#include "InputManager.h"

namespace IceEngine {
	
	static std::array<MyVertex, 4> CreateQuad(float x, float y, float textureID) {
		float size = 1.0f;

		MyVertex v0;
		v0.Position		= { x, y, 0.0f };
		v0.Color		= { 0.18f, 0.6f, 0.96f };
		v0.TexCoords	= { 0.0f, 0.0f };
		v0.TexID		= textureID;
		
		MyVertex v1;
		v1.Position		= { x + size, y, 0.0f };
		v1.Color		= { 0.18f, 0.6f, 0.96f };
		v1.TexCoords	= { 1.0f, 0.0f };
		v1.TexID		= textureID;

		MyVertex v2;
		v2.Position		= { x + size, y + size, 0.0f };
		v2.Color		= { 0.18f, 0.6f, 0.96f };
		v2.TexCoords	= { 1.0f, 1.0f };
		v2.TexID		= textureID;

		MyVertex v3;
		v3.Position		= { x, y + size, 0.0f };
		v3.Color		= { 0.18f, 0.6f, 0.96f };
		v3.TexCoords	= { 0.0f, 1.0f };
		v3.TexID		= textureID;

		return { v0, v1, v2, v3 };
	}


	static MyVertex * CreateQuad(MyVertex *target, float x, float y, float textureID) {
		if (target) {
			float size = 1.0f;

			target->Position = { x, y, 0.0f };
			target->Color = { 0.18f, 0.6f, 0.96f };
			target->TexCoords = { 0.0f, 0.0f };
			target->TexID = textureID;
			target++;
			if (!target) return target;

			target->Position = { x + size, y, 0.0f };
			target->Color = { 0.18f, 0.6f, 0.96f };
			target->TexCoords = { 1.0f, 0.0f };
			target->TexID = textureID;
			target++;
			if (!target) return target;

			target->Position = { x + size, y + size, 0.0f };
			target->Color = { 0.18f, 0.6f, 0.96f };
			target->TexCoords = { 1.0f, 1.0f };
			target->TexID = textureID;
			target++;
			if (!target) return target;

			target->Position = { x, y + size, 0.0f };
			target->Color = { 0.18f, 0.6f, 0.96f };
			target->TexCoords = { 0.0f, 1.0f };
			target->TexID = textureID;
			target++;
			if (!target) return target;
		}
		return target;
	}


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
    
		void main() {
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

		void main() {
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord);
			
		}
	)";

	void SpriteBatch::Begin() {
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

		const size_t MaxQuadCount = 1000;
		const size_t MaxVertexCount = MaxQuadCount * 4;
		const size_t MaxIndexCount = MaxQuadCount * 6;

		// Setup Vertex Buffer

		// Set up Index Buffer

		// Set up vertices for a quad with positions, colors, and texture coordinates

		//std::vector<float> vertices = {
		//	// Position					// Color				// Texture Coordinate			// Texture Index
		//	-1.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 0.0f,						0.0f,
		//	-0.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 0.0f,						0.0f,
		//	-0.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 1.0f,						0.0f,
		//	-1.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 1.0f,						0.0f,

		//	 0.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 0.0f,						1.0f,
		//	 1.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 0.0f,						1.0f,
		//	 1.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 1.0f,						1.0f,
		//	 0.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 1.0f,						1.0f
		//};

		// Assuming you have VertexBufferLayout and VertexBuffer classes
		VertexBufferLayout vertexLayout;
		vertexLayout.Push<float>(3);  // Position
		vertexLayout.Push<float>(3);  // Color
		vertexLayout.Push<float>(2);  // Texture Coordinate
		vertexLayout.Push<float>(1);  // Texture Index

		m_vertexArray.Bind();

		//m_vertexBuffer.SetData(vertices.data(), vertices.size() * sizeof(float));
		m_vertexBuffer.SetDataDynamics(sizeof(MyVertex) * 1000);

		m_vertexArray.AddBuffer(m_vertexBuffer, vertexLayout);

		// Index Buffer
		
		// Set up indices for a quad
		std::vector<unsigned int> indices = {
			//0, 1, 2, // first triangle
			//2, 3, 0, // second triangle

			//4, 5, 6,
			//6, 7, 4
		};

		for (int i = 0; i < 1000 * 4; i += 4) {
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + 2);

			indices.push_back(i + 2);
			indices.push_back(i + 3);
			indices.push_back(i);
		}

		/*
			uint32_t indices[MaxIndexCount];
			uint32_t offset =  0;
			for(size_t i = 0; i < MaxIndexCount; i += 6)
			{
				indices[i + 0] = 0 + offset;
				indices[i + 1] = 1 + offset;
				indices[i + 2] = 2 + offset;

				indices[i + 3] = 2 + offset;
				indices[i + 4] = 3 + offset;
				indices[i + 5] = 0 + offset;

				offset += 4;
			}
		*/

		// TODO(mo) set the indices to take care of any arbitrarily amount of quads.
		m_indexBuffer.SetData(indices.data(), indices.size());
		// Setup Camera
		m_cameraComponent = new IceEngine::OrthographicCameraComponent(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
		m_transformComponent = new IceEngine::TransformComponent(glm::vec2(0,0), glm::vec2(100, 100), 0.0f);
		m_cameraComponent->SetFollowPosition(m_transformComponent->position, glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		buffer = vertices.data();
	}
	

	void SpriteBatch::Render() {
		Color::SetClearColor({ 1, 71, 94 , 255 });
		glClear(GL_COLOR_BUFFER_BIT);

		static float inc = 0.1f;
	
		// Set Dynamic Vertex Buffer
		//std::vector<float> vertices = {
		//	// Position					// Color				// Texture Coordinate			// Texture Index
		//	-1.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 0.0f,						0.0f,
		//	-0.5f, -0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 0.0f,						0.0f,
		//	-0.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		1.0f, 1.0f,						0.0f,
		//	-1.5f,  0.5f, 0.0f,			0.18f, 0.6f, 0.96f,		0.0f, 1.0f,						0.0f,

		//	 0.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 0.0f,						1.0f,
		//	 1.5f, -0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 0.0f,						1.0f,
		//	 1.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		1.0f, 1.0f,						1.0f,
		//	 0.5f,  0.5f, 0.0f,			1.0f, 0.93f, 0.24f,		0.0f, 1.0f,						1.0f
		//};

		// Access violation reading location 0xFFFFFFFFFFFFFFFF.
		
		for (int y = 0; y < 5; y++) {
			for (int x = 0; x < 5; x++) {
				buffer = CreateQuad(buffer, static_cast<float>(x),
				                    static_cast<float>(y), static_cast<float>((x + y) % 2));
			}
		}


		/*auto q0 = CreateQuad(-1.5f + inc, -0.5f, 0.0f);
		auto q1 = CreateQuad(0.5f, -0.5f, 1.0f);
		auto q3 = CreateQuad(-1.5f, -0.5f, 1.0f);

		MyVertex vertices[12];
		memcpy(vertices, q0.data(), q0.size() * sizeof(MyVertex));
		memcpy(vertices + q0.size(), q1.data(), q1.size() * sizeof(MyVertex));
		memcpy(vertices + q0.size() + q1.size(), q3.data(), q3.size() * sizeof(MyVertex));*/

		m_vertexBuffer.Bind();

		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(MyVertex), vertices.data());

		// Render Vertex Array 

		m_shader->Bind();
		glBindTextureUnit(0, m_coinTextureId);
		glBindTextureUnit(1, m_playerTextureId);

		// Setup Camera View Transform
		m_shader->SetMat4("projection", m_cameraComponent->projection);
		m_shader->SetMat4("view", m_cameraComponent->GetViewMatrix());
		m_shader->SetMat4("model", m_transformComponent->GetModelMatrix());

		m_vertexArray.Bind();
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexBuffer.GetCount()), GL_UNSIGNED_INT, 0);
		
		// Unbind buffers
		m_vertexArray.Unbind();
	
		inc += 0.008f;
		buffer = vertices.data();

		m_cameraComponent->SetFollowPosition(glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	void SpriteBatch::Update(float deltaTime) {
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_L)) {
			m_cameraComponent->zoom += 0.05f;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_K)) {
			m_cameraComponent->zoom -= 0.05f;
		}
		if (IceEngine::InputManager::Instance().IsKeyDown(SDL_SCANCODE_P)) {
			m_cameraComponent->zoom = 1.0f;
		}
	}

	void SpriteBatch::End() {}
}