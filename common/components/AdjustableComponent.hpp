#pragma once

#include "SerializableComponent.hpp"

namespace kengine {
	class AdjustableComponent : public kengine::SerializableComponent<AdjustableComponent> {
	public:
		AdjustableComponent() = default;

		AdjustableComponent(const std::string & name, std::string & s) : name(name), sPtr(&s), adjustableType(String) {}
		AdjustableComponent(const std::string & name, bool & b) : name(name), bPtr(&b), adjustableType(Bool) {}
		AdjustableComponent(const std::string & name, double & d) : name(name), dPtr(&d), adjustableType(Double) {}
		AdjustableComponent(const std::string & name, int & i) : name(name), iPtr(&i), adjustableType(Int) {}

		AdjustableComponent(const std::string & name, const std::string & s) : name(name), s(s), adjustableType(String) {}
		AdjustableComponent(const std::string & name, bool b) : name(name), b(b), adjustableType(Bool) {}
		AdjustableComponent(const std::string & name, double d) : name(name), d(d), adjustableType(Double) {}
		AdjustableComponent(const std::string & name, int i) : name(name), i(i), adjustableType(Int) {}


		const std::string type = pmeta_nameof(AdjustableComponent);
		std::string name;

		std::string * sPtr = nullptr;
		bool * bPtr = nullptr;
		double * dPtr = nullptr;
		int * iPtr = nullptr;

		std::string s;
		bool b;
		double d;
		int i;

		enum EType
		{
			String,
			Bool,
			Double,
			Int
		};

		int adjustableType;

		template<typename T>
		static kengine::GameObject & create(const std::string & name, kengine::EntityManager & em, T & val) {
			return em.createEntity<kengine::GameObject>(name, [&name, &val](kengine::GameObject & go) {
				go.attachComponent<AdjustableComponent>(name, val);
			});
		}

		template<typename T>
		static kengine::GameObject & create(const std::string & name, kengine::EntityManager & em, const T & val) {
			return em.createEntity<kengine::GameObject>(name, [&name, &val](kengine::GameObject & go) {
				go.attachComponent<AdjustableComponent>(name, val);
			});
		}

	public:
		pmeta_get_class_name(AdjustableComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AdjustableComponent::type),
			pmeta_reflectible_attribute(&AdjustableComponent::name),
			pmeta_reflectible_attribute(&AdjustableComponent::s),
			pmeta_reflectible_attribute(&AdjustableComponent::s),
			pmeta_reflectible_attribute(&AdjustableComponent::b),
			pmeta_reflectible_attribute(&AdjustableComponent::b),
			pmeta_reflectible_attribute(&AdjustableComponent::d),
			pmeta_reflectible_attribute(&AdjustableComponent::d),
			pmeta_reflectible_attribute(&AdjustableComponent::i),
			pmeta_reflectible_attribute(&AdjustableComponent::i),
			pmeta_reflectible_attribute(&AdjustableComponent::adjustableType)
		);
	};
}