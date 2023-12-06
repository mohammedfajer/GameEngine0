#pragma once


#include <GL/glew.h>
#include <glm/glm.hpp>
#include "OrthographicCameraComponent.h"

namespace IceEngine
{
	
	struct ParticleProps
	{
		glm::vec2 Position;
		glm::vec2 Velocity, VelocityVariation;
		glm::vec4 ColorBegin, ColorEnd;
		float SizeBegin, SizeEnd, SizeVariation;
		float LifeTime = 1.0f;
	};


	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(float deltaTime);
		void OnRender(OrthographicCameraComponent &camera);

		void Emit(ParticleProps &particleProps);
	};


}