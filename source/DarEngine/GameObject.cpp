#include "stdafx.h"
#include "GameObject.h"


void DE::GameObject::OnUpdate(const UpdateInfo& info)
{
	for(auto& componentPtr : m_components)
	{
		componentPtr->OnUpdate(info);
	}
}

void DE::GameObject::OnDraw(const DrawInfo& info)
{
	for(auto& componentPtr: m_components)
	{
		componentPtr->OnDraw(info);
	}
}
