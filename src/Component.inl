#include "Component.hpp"
#include "GameObject.hpp"

template <typename T>
T* Component::GetComponent() const
{
	return mOwner->GetComponent<T>();
}