#pragma once

namespace IceEngine {
	class GameObject;

	struct Component {
		GameObject* owner;
		virtual void Update(float deltaTime) = 0;
		virtual ~Component() {}
	};
}
