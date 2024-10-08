#pragma once

#define IMPLEMENT_INTERFACE(Type, Interface) template<> inline const Interface##VTable Interface##VTable::value<Type> = Interface##VTable::OfType<Type>();
#define IMPLEMENT_INTERFACE1(Type, I0)				IMPLEMENT_INTERFACE(Type, I0)
#define IMPLEMENT_INTERFACE2(Type, I0, I1)			IMPLEMENT_INTERFACE(Type, I0) IMPLEMENT_INTERFACE1(Type, I1)
#define IMPLEMENT_INTERFACE3(Type, I0, I1, I2)		IMPLEMENT_INTERFACE(Type, I0) IMPLEMENT_INTERFACE2(Type, I1, I2)
#define IMPLEMENT_INTERFACE4(Type, I0, I1, I2, I3)	IMPLEMENT_INTERFACE(Type, I0) IMPLEMENT_INTERFACE3(Type, I1, I2, I3)

namespace Imagina {
	template <typename T>
	concept Interface = requires {
		typename T::_IIG_IsInterface;
	};

	class IAny {
	public:
		void *instance;
		const void *vTable;

		IAny() = default;
		IAny(const IAny &) = default;
		IAny(IAny &&) = default;
		//IAny(std::nullptr_t) : instance(nullptr), vTable(nullptr) {}
		explicit IAny(void *pointer) : instance(pointer), vTable(nullptr) {}

		IAny(void *instance, const void *vTable) : instance(instance), vTable(vTable) {}

		operator void *() { return instance; }
	};
}