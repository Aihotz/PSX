#include "Component.hpp"
#include <GameObject/GameObject.hpp>

Component::Component()
	: mOwner { nullptr }
{
}

Component::~Component()
{
}

void Component::AddToSystem()
{
}

void Component::RemoveFromSystem()
{
}

void Component::Create()
{
	AddToSystem();
}

void Component::Initialize()
{
}

void Component::Update()
{
}

void Component::Shutdown()
{
	ShutdownEvents();
	RemoveFromSystem();
}

void Component::ShutdownEvents()
{
}

GameObject* Component::GetOwner() const
{
	return mOwner;
}

void Component::Destroy()
{
	if (mOwner != nullptr)
	{
		mOwner->RemoveComponent(this);
	}
	else
	{
		Shutdown();
	}
}

#include <imgui.h>
void Component::Edit()
{
	ImGui::TextWrapped("Editing function not implemented for this component.");
}