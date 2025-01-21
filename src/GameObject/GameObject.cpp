#include "GameObject.hpp"
#include "GameObjectManager.hpp"
#include <Components/Component.hpp>
#include <Transformation/TransformationComponent.hpp>

GameObject::GameObject(const std::string& name)
	: mName { name }, mParent { nullptr }
{
}

GameObject* GameObject::FindObjectByName(const std::string& name)
{
	return GameObjectManager::GetInstance().FindObjectByName(name);
}

std::vector<GameObject*> GameObject::FindAllObjectsWithName(const std::string& name)
{
	return GameObjectManager::GetInstance().FindAllObjectsWithName(name);
}

void GameObject::SetName(const std::string& name)
{
	mName = name;
}

std::string GameObject::GetName() const
{
	return mName;
}

std::vector<Component*> GameObject::GetAllComponents() const
{
	return mComponents;
}

Component* GameObject::AddComponent(Component* component)
{
	if (component == nullptr)
	{
		return nullptr;
	}

	if (component->GetOwner() != nullptr)
	{
		return nullptr;
	}

	component->mOwner = this;
	mComponents.push_back(component);

	component->Create();

	return component;
}

void GameObject::RemoveComponent(Component* component)
{
	for (std::vector<Component*>::const_iterator it = mComponents.begin(); it != mComponents.end(); it++)
	{
		if (component != *it)
		{
			continue;
		}

		component->Shutdown();

		// memory manager
		delete component;

		mComponents.erase(it);

		return;
	}
}

void GameObject::InternalDestroy()
{
	for (GameObject* child : mChildren)
		child->Shutdown();

	ShutdownEvents();

	for (Component* component : mComponents)
	{
		component->Shutdown();

		// memory manager
		delete component;
	}

	mChildren.clear();
	mComponents.clear();
}

void GameObject::Initialize()
{
	for (Component* component : mComponents)
		component->Initialize();

	for (GameObject* child : mChildren)
		child->Initialize();

	if (mParent == nullptr)
		GameObjectManager::GetInstance().AddGameObject(this);
}

void GameObject::Shutdown()
{
	GameObjectManager::GetInstance().DestroyGameObject(this);
}

void GameObject::ShutdownEvents()
{
	for (Component* component : mComponents)
		component->ShutdownEvents();
}

void GameObject::DetachChild(GameObject* child)
{
	if (child == nullptr)
	{
		return;
	}

	for (std::vector<GameObject*>::const_iterator it = mChildren.begin(); it != mChildren.end(); it++)
	{
		if (*it == child)
		{
			mChildren.erase(it);
			child->MakeParent();
			return;
		}
	}
}

GameObject* GameObject::GetParent() const
{
	return mParent;
}

void GameObject::SetParent(GameObject* parent)
{
	// detach from current parent
	if (mParent != nullptr)
	{
		mParent->DetachChild(this);
	}

	// if previously an orfan and now has a parent
	if (mParent == nullptr && parent != nullptr)
	{
		GameObjectManager::GetInstance().RemoveRootGameObject(this);
	}

	// similarly, if the object becomes orfan
	else if (mParent != nullptr && parent == nullptr)
	{
		GameObjectManager::GetInstance().AddGameObject(this);
	}

	mParent = parent;

	// if the object has a transformation
	// set the parent transformation accordingly
	if (TransformationComponent* transformation_component = GetComponent<TransformationComponent>())
	{
		if (mParent == nullptr)
		{
			transformation_component->SetParent(nullptr);
		}
		else
		{
			transformation_component->SetParent(mParent->GetComponent<TransformationComponent>());
		}
	}
}

void GameObject::MakeParent()
{
	SetParent(nullptr);
}

void GameObject::AddChild(GameObject* child)
{
	if (child == nullptr)
	{
		return;
	}

	if (child == this)
	{
		return;
	}

	GameObjectManager::GetInstance().RemoveRootGameObject(child);

	mChildren.push_back(child);
	child->SetParent(this);
}

GameObject* GameObject::NewChild(const std::string& name)
{
	GameObject* new_child = new GameObject { name };
	AddChild(new_child);
	return new_child;
}

void GameObject::RemoveChild(GameObject* child)
{
	DetachChild(child);
	child->Shutdown();
	delete child;
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return mChildren;
}

GameObject* GameObject::GetChild(size_t index) const
{
	if (index < mChildren.size())
	{
		return mChildren[index];
	}

	return nullptr;
}

void GameObject::SetEditTarget(GameObject* object)
{
	edit_target = object;
}

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

void GameObject::Edit()
{
	if (edit_target == nullptr)
	{
		return;
	}

	ImGui::InputText("Name", &edit_target->mName);

	int id = 0;
	for (Component* component : edit_target->mComponents)
	{
		ImGui::PushID(id++);

		ImGui::Separator();
		std::string component_name = typeid(*component).name();
		if (ImGui::TreeNodeEx(
				component_name.substr(component_name.find_last_of(' ') + 1).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			component->Edit();
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}