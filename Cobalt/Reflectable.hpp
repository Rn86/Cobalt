#ifndef COBALT_REFLECTABLE_HPP_INCLUDED
#define COBALT_REFLECTABLE_HPP_INCLUDED

#include <Cobalt/TypeInfo.hpp>

namespace Cobalt
{
	struct IReflectable
	{
		virtual TypeInfo GetTypeOf() const = 0;
	};

	template <typename T>
	struct Reflectable //: virtual IReflectable
	{
		virtual TypeInfo GetTypeOf() const
		{
			return TypeOf<T>();
		}
	};
}

#endif //!COBALT_REFLECTABLE_HPP_INCLUDED