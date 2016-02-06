#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	struct ParameterInfo::Impl
	{
		Impl(const std::string && name, const TypeInfo & type)
			: m_name(std::move(name)),
			m_type(type)
		{
		}

		std::string m_name;
		TypeInfo m_type;
	};

	ParameterInfo::ParameterInfo(const std::string && name, const TypeInfo & type)
		: m_pImpl(new Impl(std::move(name), type))
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

	TypeInfo ParameterInfo::GetParameterType() const
	{
		return m_pImpl->m_type;
	}

	bool ParameterInfo::operator==(const ParameterInfo & parameter) const
	{
		return m_pImpl->m_name == parameter.GetName() &&
			m_pImpl->m_type == parameter.GetParameterType();
	}

	bool ParameterInfo::operator==(const ParameterInfo && parameter) const
	{
		return m_pImpl->m_name == parameter.GetName() &&
			m_pImpl->m_type == parameter.GetParameterType();
	}

	bool ParameterInfo::operator!=(const ParameterInfo & parameter) const
	{
		return m_pImpl->m_name != parameter.GetName() ||
			m_pImpl->m_type != parameter.GetParameterType();
	}

	bool ParameterInfo::operator!=(const ParameterInfo && parameter) const
	{
		return m_pImpl->m_name != parameter.GetName() ||
			m_pImpl->m_type != parameter.GetParameterType();
	}

	ParameterInfo & ParameterInfo::operator=(const ParameterInfo & parameter)
	{
		m_pImpl = parameter.m_pImpl;
		return *this;
	}

	ParameterInfo & ParameterInfo::operator=(const ParameterInfo && parameter) noexcept
	{
		m_pImpl = std::move(parameter.m_pImpl);
		return *this;
	}

	TypeInfo ParameterInfo::GetType() const
	{
		return Cobalt::TypeOf<ParameterInfo>();
	}

	void ParameterInfo::TypeOf(TypeRegistry<ParameterInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("ParameterInfo");
		reg.Constructor<const std::string &&, const TypeInfo &>({ "name", "type" });
		reg.Constructor<const ParameterInfo &>({ "parameter" });
		reg.Constructor<const ParameterInfo &&>({ "parameter" });
		reg.Method("GetName", &ParameterInfo::GetName);
		reg.Method("GetParameterType", &ParameterInfo::GetParameterType);
	}
}