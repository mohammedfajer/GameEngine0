#pragma once
#include "Scene.h"

namespace TopDownShooter {
	struct AnimationScene : public IceEngine::Scene {
		AnimationScene();
		~AnimationScene();
		void Update(float deltaTime) override;
		void Render() override;
	};
}

