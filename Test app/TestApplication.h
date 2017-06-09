#pragma once

#include "ADirectXApplication.h"

class TestApplication : public ADirectXApplication
{
public:
	explicit TestApplication(HINSTANCE instanceHandle);

protected:
	void OnUpdate(float deltaTime) override;
	void OnRender(float deltaTIme) override;
	void OnApplicationInitialization() override;
	void OnApplicationQuit() override;
};

