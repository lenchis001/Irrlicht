#pragma once

#include "string"
#include "map"
#include "exception"

#include "boost/any.hpp"
#include "boost/smart_ptr.hpp"


class IDiContainer {
public:
	template<class T> boost::shared_ptr<T> resolve() {
		if (_registrations.find(typeid(boost::shared_ptr<T>).hash_code()) != _registrations.end()) {
			return boost::any_cast<boost::shared_ptr<T>>(
				_registrations[typeid(boost::shared_ptr<T>).hash_code()]
			);
		}
		else {
			throw std::exception(
				(std::string("Connot find registration for ") + std::string(typeid(boost::shared_ptr<T>).name())).c_str()
			);
		}
	}

	template<class T> void registerSingleton(boost::shared_ptr<T> instance) {
		_registrations.insert(std::pair(typeid(instance).hash_code(), instance));
	}

	static boost::shared_ptr<IDiContainer> createContainer();
private:
	std::map<size_t, boost::any> _registrations;
};