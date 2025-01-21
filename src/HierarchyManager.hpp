#ifndef HIERARCHYMANAGER_HPP
#define HIERARCHYMANAGER_HPP

#include "Singleton.hpp"
#include <list>

class TransformationComponent;

class HierarchyManager : public Singleton<HierarchyManager>
{
		std::list<TransformationComponent*> mRootTransformations;

	public:
		void Update();
		void Shutdown();

		void AddComponent(TransformationComponent* component);
		void RemoveComponent(TransformationComponent* component);
};

#endif