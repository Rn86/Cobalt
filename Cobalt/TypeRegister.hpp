#ifndef COBALT_TYPE_REGISTER_HPP_INCLUDED
#define COBALT_TYPE_REGISTER_HPP_INCLUDED

namespace Cobalt
{
	struct TypeInfo;

	struct TypeRegister
	{
		template <typename T>
		static TypeInfo Register(TypeInfo(*typeof)())
		{
			return Register(typeid(T).hash_code(), typeof);
		}

		static TypeInfo Register(size_t hash, TypeInfo (*typeof)());
	};
}

#endif //!COBALT_TYPE_REGISTER_HPP_INCLUDED