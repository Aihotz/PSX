#include "Component.hpp"
#include <GameObject/GameObject.hpp>

template <typename T>
T* Component::GetComponent() const
{
	return mOwner->GetComponent<T>();
}