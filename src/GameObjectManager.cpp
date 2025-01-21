#include "GameObjectManager.hpp"
#include "GameObject.hpp"

void GameObjectManager::Update()
{
	for (GameObject* object : mObjectsMarkedAsDead)
	{
		object->InternalDestroy();
		RemoveRootGameObject(object);
	}

	mObjectsMarkedAsDead.clear();
}

void GameObjectManager::RemoveRootGameObject(GameObject* object)
{
	if (object == nullptr)
	{
		return;
	}


	mAllRootObjects.remove(object);
}

void GameObjectManager::Shutdown()
{
	for (GameObject* object : mAllRootObjects)
		object->ShutdownEvents();

	for (GameObject* object : mAllRootObjects)
	{
		object->InternalDestroy();

		// memory manager!
		delete object;
	}

	mAllRootObjects.clear();
	mObjectsMarkedAsDead.clear();
}

void GameObjectManager::DestroyGameObject(GameObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	// do not add an object twice
	if (std::find(mObjectsMarkedAsDead.begin(), mObjectsMarkedAsDead.end(), object) == mObjectsMarkedAsDead.end())
	{
		mObjectsMarkedAsDead.push_back(object);
	}
}

GameObject* GameObjectManager::NewGameObject(const std::string& name)
{
	GameObject* new_object = new GameObject { name };
	mAllRootObjects.push_back(new_object);
	return new_object;
}

GameObject* GameObjectManager::AddGameObject(GameObject* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}

	if (std::find(mAllRootObjects.begin(), mAllRootObjects.end(), object) == mAllRootObjects.end())
	{
		mAllRootObjects.push_back(object);
	}

	return object;
}

GameObject* FindObjectByNameRecursive(const std::string& name, GameObject* object)
{
	if (object == nullptr)
	{
		return nullptr;
	}

	if (object->GetName() == name)
	{
		return object;
	}

	const std::vector<GameObject*>& children = object->GetChildren();
	for (GameObject* child : children)
	{
		GameObject* found_object = FindObjectByNameRecursive(name, child);
		if (found_object != nullptr)
		{
			return found_object;
		}
	}

	return nullptr;
}

GameObject* GameObjectManager::FindObjectByName(const std::string& name) const
{
	for (GameObject* object : mAllRootObjects)
	{
		GameObject* found_object = FindObjectByNameRecursive(name, object);
		if (found_object != nullptr)
		{
			return found_object;
		}
	}

	return nullptr;
}

void FindAllObjectWithNameRecursive(const std::string& name, GameObject* object, std::vector<GameObject*>& all_objects)
{
	if (object == nullptr)
	{
		return;
	}

	if (object->GetName() == name)
	{
		all_objects.push_back(object);
	}

	const std::vector<GameObject*>& children = object->GetChildren();
	for (GameObject* child : children)
	{
		FindAllObjectWithNameRecursive(name, child, all_objects);
	}
}

std::vector<GameObject*> GameObjectManager::FindAllObjectsWithName(const std::string& name) const
{
	std::vector<GameObject*> all_objects;

	for (GameObject* object : mAllRootObjects)
	{
		FindAllObjectWithNameRecursive(name, object, all_objects);
	}

	return all_objects;
}