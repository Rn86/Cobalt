#ifndef COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED
#define COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct ParameterInfo;
	struct Value;

	struct ConstructorInfo
	{
	public:
		typedef std::function<Value(const std::vector<Value> &&)> accessor_t;

	public:
		ConstructorInfo(const std::vector<ParameterInfo> & parameters, const accessor_t & accessor);
		ConstructorInfo(const ConstructorInfo & constructor);
		ConstructorInfo(const ConstructorInfo && constructor) noexcept;

		std::vector<ParameterInfo> GetParameters() const;

		template <typename T, typename ... ARGS>
		T Invoke(ARGS ... args) const
		{
			return Invoke({ Value(std::forward<ARGS>(args))... }).GetValue<T>();
		}

		Value Invoke(const std::vector<Value> && values) const;

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_CONSTRUCTOR_INFO_HPP_INCLUDED