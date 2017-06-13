#pragma once

#include "Application/ADirectXApplication.h"

class TestApplication : public ADirectXApplication
{
public:
	explicit TestApplication(HINSTANCE instanceHandle);

protected:
	void OnUpdate(float deltaTime) override;
	void OnRender(float deltaTime) override;
	void OnApplicationInitialization() override;
	void OnApplicationQuit() override;
};

