#ifndef COBALT_METHOD_INFO_HPP_INCLUDED
#define COBALT_METHOD_INFO_HPP_INCLUDED

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct ParameterInfo;
	struct Value;

	struct MethodInfo
	{
	public:
		typedef std::function<Value(const std::vector<Value> &&)> accessor_t;

	public:
		MethodInfo(const std::vector<ParameterInfo> & parameters, const accessor_t & accessor);
		MethodInfo(const MethodInfo & constructor);
		MethodInfo(const MethodInfo && constructor) noexcept;

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

#endif //!COBALT_METHOD_INFO_HPP_INCLUDED