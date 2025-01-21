#ifndef COMPONENT_HPP
#define COMPONENT_HPP

class GameObject;

class Component
{
		friend class GameObject;
		GameObject* mOwner;

	public:
		Component();
		Component(const Component&)			   = delete;
		Component& operator=(const Component&) = delete;

		virtual ~Component() = 0;

		virtual void AddToSystem();
		virtual void RemoveFromSystem();

		virtual void Create();
		virtual void Initialize();
		virtual void Update();
		virtual void Shutdown();
		virtual void ShutdownEvents();
		
		virtual void Edit();

		GameObject* GetOwner() const;
		void		Destroy();

		template <typename T>
		T* GetComponent() const;
};

#include "Component.inl"

#endif