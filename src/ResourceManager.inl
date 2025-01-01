#include "ResourceManager.hpp"

#include <stdexcept>
#include <iostream>

template <typename T>
ResourceManager::InternalResource<T>::InternalResource(const std::string& filepath)
	: rawResource { filepath }
	, filePath { filepath }
	, id { ID_generator++ } {
		std::cout << "Constructing object of type " << typeid(T).name() << " with id " << id << std::endl;
	};

template <typename T>
ResourceManager::InternalResource<T>::~InternalResource()
{
	std::cout << "Destructing object of type " << typeid(T).name() << " with id " << id << std::endl;
}

template <typename T>
std::weak_ptr<ResourceManager::InternalResource<T>>
ResourceManager::ResourceContainer<T>::operator[](const std::string& key)
{
	typename std::unordered_map<std::string, std::shared_ptr<InternalResource<T>>>::iterator it = container.find(key);
	if (it == container.end())
	{
		container[key] = std::make_shared<InternalResource<T>>(key);
		return std::weak_ptr<InternalResource<T>>(container[key]);
	}

	return std::weak_ptr<InternalResource<T>>(it->second);
}

template <typename T>
bool ResourceManager::ResourceContainer<T>::contains(const std::string& key) const
{
	return container.contains(key);
}

template <typename T>
void ResourceManager::ResourceContainer<T>::erase(const std::string& key)
{
	container.erase(key);
}

template <typename T>
void ResourceManager::ResourceContainer<T>::clear()
{
	container.clear();
}

template <typename T>
size_t ResourceManager::ResourceContainer<T>::size() const
{
	return container.size();
}

template <typename T>
bool ResourceManager::ResourceContainer<T>::empty() const
{
	return container.empty();
}

template <typename T>
std::vector<std::string> ResourceManager::ResourceContainer<T>::keys() const
{
	std::vector<std::string> keys;

	for (typename std::unordered_map<std::string, std::shared_ptr<InternalResource<T>>>::const_iterator it
		 = container.begin();
		 it != container.end();
		 it++)
	{
		keys.push_back(it->first);
	}

	return keys;
}

template <typename T>
std::vector<std::weak_ptr<ResourceManager::InternalResource<T>>> ResourceManager::ResourceContainer<T>::values() const
{
	std::vector<std::weak_ptr<ResourceManager::InternalResource<T>>> vals;

	for (typename std::unordered_map<std::string, std::shared_ptr<InternalResource<T>>>::const_iterator it
		 = container.begin();
		 it != container.end();
		 it++)
	{
		vals.push_back(std::weak_ptr<InternalResource<T>>(it->second));
	}

	return vals;
}

template <typename T>
ResourceManager::ResourceContainer<T>& ResourceManager::GetContainer()
{
	std::type_index type = typeid(T);

	// do not add a container for a type that is already being stored
	std::unordered_map<std::type_index, std::unique_ptr<IResourceContainer>>::const_iterator it
		= resourceContainers.find(type);
	if (it != resourceContainers.end())
		return *static_cast<ResourceContainer<T>*>(it->second.get());

	// create an empty container of the types specified
	resourceContainers[type] = std::make_unique<ResourceContainer<T>>();
	return *static_cast<ResourceContainer<T>*>(resourceContainers[type].get());
}

template <typename T>
Resource<T> ResourceManager::GetResource(const std::string& name)
{
	// will create one if it does not exist
	ResourceContainer<T>& container = GetContainer<T>();

	// will try to load the file
	return Resource<T>(container[name]);
}

template <typename T>
bool ResourceManager::IsResourceLoaded(const std::string& name)
{
	std::unordered_map<std::type_index, std::unique_ptr<IResourceContainer>>::const_iterator it
		= resourceContainers.find(typeid(T));

	if (it == resourceContainers.end())
		return false;

	ResourceContainer<T>& container = *static_cast<ResourceContainer<T>*>(it->second.get());
	return container.contains(name);
}

template <typename T>
void ResourceManager::DeleteResources()
{
	resourceContainers.erase(typeid(T));
}

template <typename T>
Resource<T>::Resource(const std::weak_ptr<ResourceManager::InternalResource<T>>& reference)
	: resourceReference { reference }
{
}

template <typename T>
Resource<T>::Resource(const std::string& name)
{
	Resource<T> tempResource = ResourceManager::GetInstance().GetResource<T>(name);
	resourceReference		 = tempResource.resourceReference;
}

template <typename T>
void Resource<T>::Load(const std::string& name)
{
	Resource<T> tempResource = ResourceManager::GetInstance().GetResource<T>(name);
	resourceReference		 = tempResource.resourceReference;
}

template <typename T>
T* Resource<T>::operator->()
{
	std::shared_ptr<ResourceManager::InternalResource<T>> internalResourceReference = resourceReference.lock();
	if (internalResourceReference == nullptr)
	{
		throw std::runtime_error("Resource is not available available.");
	}

	return &internalResourceReference->rawResource;
}

template <typename T>
bool Resource<T>::IsValid() const
{
	return resourceReference.lock() != nullptr;
}