#include "GameObject.hpp"

template <typename T>
T* GameObject::GetComponent() const
{
	for (Component* component : mComponents)
	{
		if (T* casted_component = dynamic_cast<T*>(component))
		{
			return casted_component;
		}
	}

	return nullptr;
}

template <typename T>
std::vector<T*> GameObject::GetAllComponents() const
{
	std::vector<T*> components;

	for (Component* component : mComponents)
	{
		if (T* casted_component = dynamic_cast<T*>(component))
		{
			components.push_back(casted_component);
		}
	}

	return components;
}

template <typename T>
T* GameObject::AddComponent()
{
	// at some point, move this to a memory manager
	return static_cast<T*>(AddComponent(new T {}));
}

template <typename T>
void GameObject::RemoveComponent()
{
	RemoveComponent(GetComponent<T>());
}

template <typename T>
void GameObject::RemoveAllComponents()
{
	std::vector<T*> components = GetAllComponents<T>();
	for (T* component : components)
	{
		RemoveComponent(component);
	}
}