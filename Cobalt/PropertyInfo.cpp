#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	struct PropertyInfo::Impl
	{
		Impl(const std::string && name, const MethodInfo & getter, const MethodInfo & setter)
			: m_name(std::move(name)),
			m_getter(getter),
			m_setter(setter)
		{
		}

		std::string m_name;
		MethodInfo m_getter;
		MethodInfo m_setter;
	};

	PropertyInfo::PropertyInfo(const std::string && name, const MethodInfo & getter, const MethodInfo & setter)
		: m_pImpl(new Impl(std::move(name), getter, setter))
	{
	}

	PropertyInfo::PropertyInfo(const PropertyInfo & property)
		: m_pImpl(property.m_pImpl)
	{
	}

	PropertyInfo::PropertyInfo(const PropertyInfo && property) noexcept
		: m_pImpl(std::move(property.m_pImpl))
	{
	}

	std::string PropertyInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	TypeInfo PropertyInfo::GetType() const
	{
		return m_pImpl->m_getter.GetReturnType();
	}

	MethodInfo PropertyInfo::GetGetMethod() const
	{
		return m_pImpl->m_getter;
	}

	MethodInfo PropertyInfo::GetSetMethod() const
	{
		return m_pImpl->m_setter;
	}

	bool PropertyInfo::operator==(const PropertyInfo & property) const
	{
		return m_pImpl->m_getter == property.GetGetMethod() &&
			m_pImpl->m_setter == property.GetSetMethod();
	}

	bool PropertyInfo::operator==(const PropertyInfo && property) const
	{
		return m_pImpl->m_getter == property.GetGetMethod() &&
			m_pImpl->m_setter == property.GetSetMethod();
	}

	bool PropertyInfo::operator!=(const PropertyInfo & property) const
	{
		return m_pImpl->m_getter != property.GetGetMethod() ||
			m_pImpl->m_setter != property.GetSetMethod();
	}

	bool PropertyInfo::operator!=(const PropertyInfo && property) const
	{
		return m_pImpl->m_getter != property.GetGetMethod() ||
			m_pImpl->m_setter != property.GetSetMethod();
	}

	PropertyInfo & PropertyInfo::operator=(const PropertyInfo & property)
	{
		m_pImpl = property.m_pImpl;
		return *this;
	}

	PropertyInfo & PropertyInfo::operator=(const PropertyInfo && property) noexcept
	{
		m_pImpl = std::move(property.m_pImpl);
		return *this;
	}

	void PropertyInfo::TypeOf(TypeRegistry<PropertyInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("PropertyInfo");
		reg.Constructor<const std::string &&, const MethodInfo &, const MethodInfo &>({ "name", "getter", "setter" });
		reg.Constructor<const PropertyInfo &>({ "property" });
		reg.Constructor<const PropertyInfo &&>({ "property" });
		reg.Method("GetName", &PropertyInfo::GetName);
		reg.Method("GetType", &PropertyInfo::GetType);
		reg.Method("GetGetMethod", &PropertyInfo::GetGetMethod);
		reg.Method("GetSetMethod", &PropertyInfo::GetSetMethod);
	}
}