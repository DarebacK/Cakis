#include "stdafx.h"
#include "GameObject.h"


void DE::GameObject::OnUpdate(const UpdateInfo& info)
{
	for(auto& component : m_components)
	{
		component.OnUpdate(info);
	}
}

void DE::GameObject::OnDraw(const DrawInfo& info)
{
	for(auto& component: m_components)
	{
		component.OnDraw(info);
	}
}
