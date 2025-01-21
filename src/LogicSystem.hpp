#ifndef LOGICSYSTEM_HPP
#define LOGICSYSTEM_HPP

#include "Singleton.hpp"
#include <list>

class LogicComponent;

class LogicSystem : public Singleton<LogicSystem>
{
		std::list<LogicComponent*> mLogicComponents;

	public:
		void Update();
		void Shutdown();

		void AddComponent(LogicComponent* component);
		void RemoveComponent(LogicComponent* component);
};

#endif