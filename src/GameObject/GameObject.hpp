#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <vector>
#include <string>

class Component;
class GameObjectManager;

class GameObject
{
		friend class GameObjectManager;

		std::string mName;

		GameObject*				 mParent;
		std::vector<GameObject*> mChildren;

		std::vector<Component*> mComponents;

		GameObject(const std::string& name = "Game Object");

		void InternalDestroy();
		void DetachChild(GameObject* child);

		static inline GameObject* edit_target = nullptr;

	public:
		static GameObject*				FindObjectByName(const std::string& name);
		static std::vector<GameObject*> FindAllObjectsWithName(const std::string& name);

		void		SetName(const std::string& name);
		std::string GetName() const;

		void Initialize();
		void Shutdown();
		void ShutdownEvents();

		GameObject* GetParent() const;
		void		SetParent(GameObject* parent);
		void		MakeParent();

		void							AddChild(GameObject* child);
		GameObject*						NewChild(const std::string& name = "Game Object");
		void							RemoveChild(GameObject* child);
		const std::vector<GameObject*>& GetChildren() const;
		GameObject*						GetChild(size_t index) const;

		template <typename T>
		T* GetComponent() const;

		template <typename T>
		std::vector<T*> GetAllComponents() const;

		std::vector<Component*> GetAllComponents() const;

		template <typename T>
		T* AddComponent();

		Component* AddComponent(Component* component);

		template <typename T>
		void RemoveComponent();

		template <typename T>
		void RemoveAllComponents();

		void RemoveComponent(Component* component);

		static void SetEditTarget(GameObject* object);
		static void Edit();
};

#include "GameObject.inl"

#endif