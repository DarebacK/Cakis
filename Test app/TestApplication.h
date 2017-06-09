#pragma once

#include "ADirectXApplication.h"

class TestApplication : public ADirectXApplication
{
public:
	explicit TestApplication(HINSTANCE instanceHandle);

	bool Update(float deltaTime) override;
	bool Render(float deltaTIme) override;
protected:
	void OnApplicationInitialization() override;
	void OnApplicationQuit() override;
};

