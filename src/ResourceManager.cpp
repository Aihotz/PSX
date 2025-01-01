#include "ResourceManager.hpp"

void ResourceManager::DeleteResources()
{
	resourceContainers.clear();
}

void ResourceManager::Shutdown()
{
	DeleteResources();
}