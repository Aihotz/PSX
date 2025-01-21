#include "LogicComponent.hpp"
#include "LogicSystem.hpp"

LogicComponent::~LogicComponent()
{
}

void LogicComponent::AddToSystem()
{
	LogicSystem::GetInstance().AddComponent(this);
}

void LogicComponent::RemoveFromSystem()
{
	LogicSystem::GetInstance().RemoveComponent(this);
}

void LogicComponent::Create()
{
	Component::Create();
}

void LogicComponent::Shutdown()
{
	OnDestroy();

	Component::Shutdown();
}

void LogicComponent::OnDestroy()
{
}