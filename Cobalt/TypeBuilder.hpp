#ifndef COBALT_TYPE_BUILDER_HPP_INCLUDED
#define COBALT_TYPE_BUILDER_HPP_INCLUDED

#include <Cobalt/TypeInfo.hpp>

#include <functional>
#include <string>

namespace Cobalt
{
	struct TypeBuilder
	{
	public:
		TypeBuilder(const std::string && space, const std::string && name);
		TypeBuilder(const TypeBuilder & builder);
		TypeBuilder(const TypeBuilder && builder) noexcept;

		FieldInfo DefineField(const std::string && name, const TypeInfo && type, const std::function<Object(const Object &&)> && accessor);

		template <typename R, typename T>
		FieldInfo DefineField(const std::string && name, const std::function<R(const T &)> && accessor)
		{
			return DefineField(std::move(name), Cobalt::TypeOf<R>(), [accessor](const Object && object)
			{
				return Object(accessor(object.As<T>()));
			});
		}

		TypeInfo CreateType() const;

		TypeBuilder & operator=(const TypeBuilder & builder);
		TypeBuilder & operator=(const TypeBuilder && builder) noexcept;

	private:
		friend struct Access;
		virtual TypeInfo GetType() const;
		static void TypeOf(TypeRegistry<PropertyInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_TYPE_BUILDER_HPP_INCLUDED
