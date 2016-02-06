#include <Cobalt/Reflection.hpp>
#include <Cobalt/TypeBuilder.hpp>

namespace Cobalt
{
	struct TypeBuilder::Impl
	{
		Impl(const std::string && space, const std::string && name)
		{
			m_parameters.m_space = std::move(space);
			m_parameters.m_name = std::move(name);
		}

		TypeInfoParameters m_parameters;
	};

	TypeBuilder::TypeBuilder(const std::string && space, const std::string && name)
		: m_pImpl(new Impl(std::move(space), std::move(name)))
	{
	}

	TypeBuilder::TypeBuilder(const TypeBuilder & builder)
		: m_pImpl(builder.m_pImpl)
	{
	}

	TypeBuilder::TypeBuilder(const TypeBuilder && builder) noexcept
		: m_pImpl(std::move(builder.m_pImpl))
	{
	}

	FieldInfo TypeBuilder::DefineField(const std::string && name, const TypeInfo && type, const std::function<Object(const Object &&)> && accessor)
	{
		FieldInfo field(std::move(name), std::move(type), std::move(accessor));
		m_pImpl->m_parameters.m_fields.push_back(field);
		return field;
	}

	TypeInfo TypeBuilder::CreateType() const
	{
		return TypeInfo(m_pImpl->m_parameters);
	}

	TypeBuilder & TypeBuilder::operator=(const TypeBuilder & oper)
	{
		m_pImpl = oper.m_pImpl;
		return *this;
	}

	TypeBuilder & TypeBuilder::operator=(const TypeBuilder && builder) noexcept
	{
		m_pImpl = std::move(builder.m_pImpl);
		return *this;
	}

	TypeInfo TypeBuilder::GetType() const
	{
		return Cobalt::TypeOf<TypeBuilder>();
	}

	void TypeBuilder::TypeOf(TypeRegistry<PropertyInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("TypeBuilder");
	}
}
