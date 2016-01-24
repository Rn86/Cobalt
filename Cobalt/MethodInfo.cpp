#include <Cobalt/MethodInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/Value.hpp>

namespace Cobalt
{
	struct MethodInfo::Impl
	{
		Impl(const std::vector<ParameterInfo> & parameters, const MethodInfo::accessor_t & accessor)
			: m_parameters(parameters),
			m_accessor(accessor)
		{
		}

		std::vector<ParameterInfo> m_parameters;
		MethodInfo::accessor_t m_accessor;
	};

	MethodInfo::MethodInfo(const std::vector<ParameterInfo> & parameters, const accessor_t & accessor)
		: m_pImpl(new Impl(parameters, accessor))
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

	std::vector<ParameterInfo> MethodInfo::GetParameters() const
	{
		return m_pImpl->m_parameters;
	}

	Value MethodInfo::Invoke(const std::vector<Value> && values) const
	{
		return m_pImpl->m_accessor(std::move(values));
	}
}