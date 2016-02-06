#ifndef COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED
#define COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct ParameterInfo;
	struct Object;

	struct ConstructorInfo
	{
	public:
		typedef std::function<Object(const std::vector<Object> &&)> accessor_t;

	public:
		ConstructorInfo(const std::vector<ParameterInfo> & parameters, const accessor_t & accessor);
		ConstructorInfo(const ConstructorInfo & constructor);
		ConstructorInfo(const ConstructorInfo && constructor) noexcept;

		std::vector<ParameterInfo> GetParameters() const;

		template <typename T, typename ... ARGS>
		T Invoke(ARGS ... args) const
		{
			return Invoke({ Object(std::forward<ARGS>(args))... }).As<T>();
		}

		Object Invoke(const std::vector<Object> && arguments) const;

		bool operator==(const ConstructorInfo & constructor) const;
		bool operator==(const ConstructorInfo && constructor) const;
		bool operator!=(const ConstructorInfo & constructor) const;
		bool operator!=(const ConstructorInfo && constructor) const;
		ConstructorInfo & operator=(const ConstructorInfo & constructor);
		ConstructorInfo & operator=(const ConstructorInfo && constructor) noexcept;

	private:
		friend struct Access;
		static void TypeOf(TypeRegistry<ConstructorInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED