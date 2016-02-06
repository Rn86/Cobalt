#ifndef COBALT_METHOD_INFO_HPP_INCLUDED
#define COBALT_METHOD_INFO_HPP_INCLUDED

#include <Cobalt/Forward.hpp>

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct ParameterInfo;
	struct Object;

	enum MethodModifier
	{
		mmNone = 0,
		mmConst = 1,
		mmVolatile = 2
	};

	struct MethodInfo
	{
	public:
		typedef std::function<Object(const Object &&, const std::vector<Object> &&)> accessor_t;

	public:
		MethodInfo(const TypeInfo & returnType, const std::string && name, const std::vector<ParameterInfo> & parameters, MethodModifier modifier, bool isStatic, const accessor_t & accessor);
		MethodInfo(const MethodInfo & constructor);
		MethodInfo(const MethodInfo && constructor) noexcept;

		TypeInfo GetReturnType() const;
		std::string GetName() const;
		std::vector<ParameterInfo> GetParameters() const;
		MethodModifier GetModifier() const;
		bool IsStatic() const;

		Object Invoke(const Object & object, const std::vector<Object> && arguments) const;
		Object Invoke(const Object && object, const std::vector<Object> && arguments) const;

		template <typename T, typename O, typename ... ARGS>
		T Invoke(const O & object, ARGS ... args) const
		{
			return Invoke(Object(object), { Object(std::forward<ARGS>(args))... }).As<T>();
		}

		bool operator==(const MethodInfo & method) const;
		bool operator==(const MethodInfo && method) const;
		bool operator!=(const MethodInfo & method) const;
		bool operator!=(const MethodInfo && method) const;
		MethodInfo & operator=(const MethodInfo & method);
		MethodInfo & operator=(const MethodInfo && method) noexcept;

	private:
		friend struct Access;
		virtual TypeInfo GetType() const;
		static void TypeOf(TypeRegistry<MethodInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_METHOD_INFO_HPP_INCLUDED