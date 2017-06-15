#pragma once

#include "Object.h"
#include "Math/Transform.h"

namespace DarEngine {
	class SceneObject : public DarEngine::Object
	{
	public:
		SceneObject(long id, const DarEngine::Transform& transform);

	private:
		DarEngine::Transform transform;
	};
}