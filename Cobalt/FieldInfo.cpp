#include <Cobalt/FieldInfo.hpp>
#include <Cobalt/TypeInfo.hpp>
#include <Cobalt/Object.hpp>

namespace Cobalt
{
	struct FieldInfo::Impl
	{
		Impl(const std::string & name, const TypeInfo & type, const std::function<Object(const Object &&)> & accessor)
			: m_name(name),
			m_type(type),
			m_accessor(accessor)
		{
		}

		std::string m_name;
		TypeInfo m_type;
		std::function<Object(const Object &&)> m_accessor;
	};

	FieldInfo::FieldInfo(const std::string & name, const TypeInfo & type, const std::function<Object(const Object &&)> & accessor)
		: m_pImpl(new Impl(name, type, accessor))
	{
	}

	FieldInfo::FieldInfo(const FieldInfo & field)
		: m_pImpl(field.m_pImpl)
	{
	}

	FieldInfo::FieldInfo(const FieldInfo && field) noexcept
		: m_pImpl(std::move(field.m_pImpl))
	{
	}

	std::string FieldInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	TypeInfo FieldInfo::GetType() const
	{
		return m_pImpl->m_type;
	}

	Object FieldInfo::Invoke(const Object && object) const
	{
		return m_pImpl->m_accessor(std::move(object));
	}
}