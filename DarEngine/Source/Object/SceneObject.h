#pragma once

#include "Object.h"

class Transform;

class SceneObject : public Object
{
public:
	SceneObject(long id, const Transform& transform);

private:
	Transform transform;
};