#include "HierarchyManager.hpp"
#include "TransformationComponent.hpp"
#include "GameObject.hpp"
#include <vector>

void HierarchyManager::Shutdown()
{
	mRootTransformations.clear();
}

void UpdateHierarchyRecursive(GameObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	TransformationComponent* component = object->GetComponent<TransformationComponent>();
	if (component == nullptr)
	{
		return;
	}

	component->Update();
	const std::vector<GameObject*>& children = object->GetChildren();
	for (GameObject* child : children)
	{
		UpdateHierarchyRecursive(child);
	}
}

void HierarchyManager::Update()
{
	for (TransformationComponent* component : mRootTransformations)
	{
		component->Update();

		const std::vector<GameObject*>& children = component->GetOwner()->GetChildren();
		for (GameObject* child : children)
		{
			UpdateHierarchyRecursive(child);
		}
	}
}

void HierarchyManager::AddComponent(TransformationComponent* component)
{
	if (component == nullptr)
	{
		return;
	}

	if (std::find(mRootTransformations.begin(), mRootTransformations.end(), component) == mRootTransformations.end())
	{
		mRootTransformations.push_back(component);
	}
}

void HierarchyManager::RemoveComponent(TransformationComponent* component)
{
	if (component == nullptr)
	{
		return;
	}

	mRootTransformations.remove(component);
}