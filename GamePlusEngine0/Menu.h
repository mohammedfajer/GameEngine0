#pragma once

#include "Scene.h"

namespace TopDownShooter
{
	class MenuScene : public IceEngine::Scene
	{
	public:
		MenuScene();
		~MenuScene();
		void Update(float deltaTime) override;
		void Render() override;
	};
}

// Menu Mode
// Game Mode
// 








