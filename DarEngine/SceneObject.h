#pragma once

#include "Object.h"
#include "Transform.h"

class SceneObject : public Object
{
public:
	SceneObject(long id, const Transform& transform);

private:
	Transform transform;
};