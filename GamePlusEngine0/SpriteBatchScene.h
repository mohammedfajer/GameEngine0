#pragma once

#include "Scene.h"
#include "SpriteBatch.h"

namespace TopDownShooter
{
	class SpriteBatchScene : public IceEngine::Scene
	{
	public:
		SpriteBatchScene();
		~SpriteBatchScene();
		void Update(float deltaTime) override;
		void Render() override;
	private:
		IceEngine::SpriteBatch* m_spriteBatch;
	};
}

