#ifndef SINGLETON_HPP
#define SINGLETON_HPP

template <typename T>
class Singleton
{
	protected:
		Singleton()	 = default;
		~Singleton() = default;

	public:
		static T& GetInstance();

		Singleton(const Singleton&)			   = delete;
		Singleton& operator=(const Singleton&) = delete;
};

#include "Singleton.inl"

#endif