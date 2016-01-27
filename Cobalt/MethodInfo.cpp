#include <Cobalt/MethodInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/Value.hpp>

namespace Cobalt
{
	struct MethodInfo::Impl
	{
		Impl(const TypeInfo & returnType, const std::vector<ParameterInfo> & parameters, MethodModifier modifier, const MethodInfo::accessor_t & accessor)
			: m_returnType(returnType),
			m_parameters(parameters),
			m_modifier(modifier),
			m_accessor(accessor)
		{
		}

		TypeInfo m_returnType;
		std::vector<ParameterInfo> m_parameters;
		MethodModifier m_modifier;
		MethodInfo::accessor_t m_accessor;
	};

	MethodInfo::MethodInfo(const TypeInfo & returnType, const std::vector<ParameterInfo> & parameters, MethodModifier modifier, const accessor_t & accessor)
		: m_pImpl(new Impl(returnType, parameters, modifier, accessor))
	{
	}

	MethodInfo::MethodInfo(const MethodInfo & method)
		: m_pImpl(method.m_pImpl)
	{
	}

	MethodInfo::MethodInfo(const MethodInfo && method) noexcept
		: m_pImpl(std::move(method.m_pImpl))
	{
	}

	TypeInfo MethodInfo::GetReturnType() const
	{
		return m_pImpl->m_returnType;
	}

	std::vector<ParameterInfo> MethodInfo::GetParameters() const
	{
		return m_pImpl->m_parameters;
	}

	MethodModifier MethodInfo::GetModifier() const
	{
		return m_pImpl->m_modifier;
	}

	Value MethodInfo::Invoke(const Value && object, const std::vector<Value> && values) const
	{
		return m_pImpl->m_accessor(std::move(object), std::move(values));
	}
}