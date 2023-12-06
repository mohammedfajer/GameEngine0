
#include "ParticlesScene.h"
#include "Renderer.h"

namespace TopDownShooter
{
	const std::string vertex_shader = R"(
		#version 330 core
		layout (location = 0) in vec3 aPos;
		layout (location = 1) in vec4 aColor;
		layout (location = 2) in vec2 aTexCoord;
		layout (location = 3) in float aTexIdx;

		out vec4 OurColor;
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

		in vec4 OurColor;
		in vec2 TexCoord;
		in float TexIdx;

		uniform sampler2D u_Textures[32];

		void main()
		{
			int index = int(TexIdx);
			FragColor = texture(u_Textures[index], TexCoord) * OurColor;
			
		}
	)";

	ParticlesScene::ParticlesScene ()
	{
		m_name = "ParticlesScene";

		m_shader = new IceEngine::Shader();
		m_shader->LoadShaderFromString(vertex_shader.c_str(), fragment_shader.c_str());
		m_shader->Bind();
		auto loc = glGetUniformLocation(m_shader->id, "u_Textures");
		int samplers[32];
		for (int i = 0; i < 32; i++) samplers[i] = i;
		glUniform1iv(loc, 32, samplers);

		IceEngine::Renderer::Init();
	}

	ParticlesScene::~ParticlesScene()
	{

	}

	void ParticlesScene::Update(float deltaTime)
	{

	}


	void ParticlesScene::Render()
	{

	}
}