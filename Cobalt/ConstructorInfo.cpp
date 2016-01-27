#include <Cobalt/ConstructorInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/Object.hpp>

namespace Cobalt
{
	struct ConstructorInfo::Impl
	{
		Impl(const std::vector<ParameterInfo> & parameters, const ConstructorInfo::accessor_t & accessor)
			: m_parameters(parameters),
			m_accessor(accessor)
		{
		}

		std::vector<ParameterInfo> m_parameters;
		ConstructorInfo::accessor_t m_accessor;
	};

	ConstructorInfo::ConstructorInfo(const std::vector<ParameterInfo> & parameters, const accessor_t & accessor)
		: m_pImpl(new Impl(parameters, accessor))
	{
	}

	ConstructorInfo::ConstructorInfo(const ConstructorInfo & constructor)
		: m_pImpl(constructor.m_pImpl)
	{
	}

	ConstructorInfo::ConstructorInfo(const ConstructorInfo && constructor) noexcept
		: m_pImpl(std::move(constructor.m_pImpl))
	{
	}

	std::vector<ParameterInfo> ConstructorInfo::GetParameters() const
	{
		return m_pImpl->m_parameters;
	}

	Object ConstructorInfo::Invoke(const std::vector<Object> && arguments) const
	{
		return m_pImpl->m_accessor(std::move(arguments));
	}
}