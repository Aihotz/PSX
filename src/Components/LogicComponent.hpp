#ifndef LOGICCOMPONENT_HPP
#define LOGICCOMPONENT_HPP

#include "Component.hpp"

class LogicComponent : public Component
{
		virtual void AddToSystem() final override;
		virtual void RemoveFromSystem() final override;
		virtual void Create() final override;
		virtual void Shutdown() final override;

	public:
		virtual ~LogicComponent() = 0;

		using Component::Initialize;
		using Component::Update;
		using Component::ShutdownEvents;
		virtual void OnDestroy();
};

#endif