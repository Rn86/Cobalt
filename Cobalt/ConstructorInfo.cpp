#include <Cobalt/Reflection.hpp>

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

	bool ConstructorInfo::operator==(const ConstructorInfo & constructor) const
	{
		return m_pImpl->m_parameters == constructor.GetParameters();
	}

	bool ConstructorInfo::operator==(const ConstructorInfo && constructor) const
	{
		return m_pImpl->m_parameters == constructor.GetParameters();
	}

	bool ConstructorInfo::operator!=(const ConstructorInfo & constructor) const
	{
		return m_pImpl->m_parameters != constructor.GetParameters();
	}

	bool ConstructorInfo::operator!=(const ConstructorInfo && constructor) const
	{
		return m_pImpl->m_parameters != constructor.GetParameters();
	}

	ConstructorInfo & ConstructorInfo::operator=(const ConstructorInfo & constructor)
	{
		m_pImpl = constructor.m_pImpl;
		return *this;
	}

	ConstructorInfo & ConstructorInfo::operator=(const ConstructorInfo && constructor) noexcept
	{
		m_pImpl = std::move(constructor.m_pImpl);
		return *this;
	}

	TypeInfo ConstructorInfo::GetType() const
	{
		return Cobalt::TypeOf<ConstructorInfo>();
	}

	void ConstructorInfo::TypeOf(TypeRegistry<ConstructorInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("ConstructorInfo");
		reg.Constructor<const std::vector<ParameterInfo> &, const accessor_t &>({ "parameters", "accessor" });
		reg.Constructor<const ConstructorInfo &>({ "constructor" });
		reg.Constructor<const ConstructorInfo &&>({ "constructor" });
		reg.Method("GetParameters", &ConstructorInfo::GetParameters);
		reg.Method("Invoke", &ConstructorInfo::Invoke);
	}
}