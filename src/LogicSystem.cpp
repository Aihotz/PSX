#include "LogicSystem.hpp"
#include "LogicComponent.hpp"

void LogicSystem::Update()
{
	for (LogicComponent* component : mLogicComponents)
	{
		component->Update();
	}
}

void LogicSystem::Shutdown()
{
	mLogicComponents.clear();
}

void LogicSystem::AddComponent(LogicComponent* component)
{
	if (component == nullptr)
	{
		return;
	}

	if (std::find(mLogicComponents.begin(), mLogicComponents.end(), component) == mLogicComponents.end())
	{
		mLogicComponents.push_back(component);
	}
}

void LogicSystem::RemoveComponent(LogicComponent* component)
{
	if (component == nullptr)
	{
		return;
	}

	mLogicComponents.remove(component);
}