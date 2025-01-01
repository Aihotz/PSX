#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include "Singleton.hpp"
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

template <typename T>
class Resource;

class ResourceManager : public Singleton<ResourceManager>
{
		// ugly, but otherwise Resource cannot
		// have a weak_ptr to InternalResource...
		template <typename T>
		friend class Resource;

		template <typename T>
		struct InternalResource
		{
				static inline int ID_generator = 0;

				T			rawResource;
				std::string filePath;
				int			id;

				InternalResource(const std::string& filepath);
				~InternalResource();
		};

		struct IResourceContainer
		{
				virtual ~IResourceContainer() = default;
		};

		template <typename T>
		class ResourceContainer : public IResourceContainer
		{
				std::unordered_map<std::string, std::shared_ptr<InternalResource<T>>> container;

			public:
				std::weak_ptr<InternalResource<T>> operator[](const std::string& key);
				bool							   contains(const std::string& key) const;

				void   erase(const std::string& key);
				void   clear();
				size_t size() const;
				bool   empty() const;

				std::vector<std::string>						keys() const;
				std::vector<std::weak_ptr<InternalResource<T>>> values() const;
		};

		// containers for every resource type
		std::unordered_map<std::type_index, std::unique_ptr<IResourceContainer>> resourceContainers;

		template <typename T>
		ResourceContainer<T>& GetContainer();

	public:
		template <typename T>
		Resource<T> GetResource(const std::string& name);

		template <typename T>
		bool IsResourceLoaded(const std::string& name);

		template <typename T>
		void DeleteResources();

		void DeleteResources();
		void Shutdown();
};

template <typename T>
class Resource
{
		// only the ResourceManager has access to weak_ptr constructor
		friend class ResourceManager;
		Resource(const std::weak_ptr<ResourceManager::InternalResource<T>>& reference);
		std::weak_ptr<ResourceManager::InternalResource<T>> resourceReference;

	public:
		Resource(const std::string& name);

		Resource()								   = default;
		Resource(const Resource& other)			   = default;
		Resource& operator=(const Resource& other) = default;

		void Load(const std::string& name);

		T*	 operator->();
		bool IsValid() const;
};

#include "ResourceManager.inl"

#endif