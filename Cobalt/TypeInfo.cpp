#include <Cobalt/TypeInfo.hpp>
#include <Cobalt/ConstructorInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	struct TypeInfo::Impl
	{
		Impl(const TypeInfoParameters & reg)
			: m_hash(reg.m_hash),
			m_space(reg.m_space),
			m_name(reg.m_name),
			m_fields(reg.m_fields),
			m_constructors(reg.m_constructors),
			m_methods(reg.m_methods)
		{
		}

		size_t m_hash;
		std::string m_name;
		std::string m_space;
		std::vector<FieldInfo> m_fields;
		std::vector<ConstructorInfo> m_constructors;
		std::vector<MethodInfo> m_methods;
	};

	TypeInfo::TypeInfo(const TypeInfoParameters & reg)
		: m_pImpl(new Impl(reg))
	{
	}

	TypeInfo::TypeInfo(const TypeInfo & info)
		: m_pImpl(info.m_pImpl)
	{
	}

	TypeInfo::TypeInfo(const TypeInfo && info) noexcept
		: m_pImpl(std::move(info.m_pImpl))
	{
	}

	size_t TypeInfo::GetHashCode() const
	{
		return m_pImpl->m_hash;
	}

	std::string TypeInfo::GetNamespace() const
	{
		return m_pImpl->m_space;
	}

	std::string TypeInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	std::vector<FieldInfo> TypeInfo::GetFields() const
	{
		return m_pImpl->m_fields;
	}

	std::vector<ConstructorInfo> TypeInfo::GetConstructors() const
	{
		return m_pImpl->m_constructors;
	}

	std::vector<MethodInfo> TypeInfo::GetMethods() const
	{
		return m_pImpl->m_methods;
	}

	ConstructorInfo TypeInfo::GetConstructor(const std::vector<TypeInfo> && types) const
	{
		bool found = false;
		for (auto constructor : m_pImpl->m_constructors)
		{
			auto parameters = constructor.GetParameters();
			for (size_t i = 0; i < parameters.size(); i++)
			{
				found = (types[i] == parameters[i].GetType());
				if (!found) break;
			}
			if (found) return constructor;
		}
		throw std::exception("Could not find appropriet constructor");
	}

	TypeInfo::operator std::string() const
	{
		return m_pImpl->m_space + "::" + m_pImpl->m_name;
	}

	bool TypeInfo::operator==(const TypeInfo & type) const
	{
		return type.GetHashCode() == GetHashCode();
	}

	bool TypeInfo::operator==(const TypeInfo && type) const
	{
		return type.GetHashCode() == GetHashCode();
	}

	bool TypeInfo::operator!=(const TypeInfo & type) const
	{
		return type.GetHashCode() != GetHashCode();
	}

	bool TypeInfo::operator!=(const TypeInfo && type) const
	{
		return type.GetHashCode() != GetHashCode();
	}

	void TypeInfo::TypeOf(TypeRegistry<TypeInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("TypeInfo");
	}
}