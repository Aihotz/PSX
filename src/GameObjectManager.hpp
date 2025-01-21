#ifndef GAMEOBJECTMANAGER_HPP
#define GAMEOBJECTMANAGER_HPP

#include "Singleton.hpp"
#include <string>
#include <list>
#include <vector>

class GameObject;

class GameObjectManager : public Singleton<GameObjectManager>
{
		friend class GameObject;

		std::list<GameObject*> mAllRootObjects;
		std::list<GameObject*> mObjectsMarkedAsDead;

		void RemoveRootGameObject(GameObject* object);

	public:
		void Update();
		void Shutdown();

		void DestroyGameObject(GameObject* object);

		GameObject* NewGameObject(const std::string& name = "Game Object");
		GameObject* AddGameObject(GameObject* object);
		GameObject* FindObjectByName(const std::string& name) const;
		std::vector<GameObject*> FindAllObjectsWithName(const std::string& name) const;
};

#endif