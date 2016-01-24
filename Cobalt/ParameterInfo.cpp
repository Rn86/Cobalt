#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/TypeInfo.hpp>

namespace Cobalt
{
	struct ParameterInfo::Impl
	{
		Impl(const std::string & name, const TypeInfo & type)
			: m_name(name),
			m_type(type)
		{
		}

		std::string m_name;
		TypeInfo m_type;
	};

	ParameterInfo::ParameterInfo(const std::string & name, const TypeInfo & type)
		: m_pImpl(new Impl(name, type))
	{
	}

	ParameterInfo::ParameterInfo(const ParameterInfo & parameter)
		: m_pImpl(parameter.m_pImpl)
	{
	}

	ParameterInfo::ParameterInfo(const ParameterInfo && parameter) noexcept
		: m_pImpl(std::move(parameter.m_pImpl))
	{
	}

	std::string ParameterInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	TypeInfo ParameterInfo::GetType() const
	{
		return m_pImpl->m_type;
	}
}