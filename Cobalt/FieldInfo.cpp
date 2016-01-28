#include <Cobalt/FieldInfo.hpp>
#include <Cobalt/TypeInfo.hpp>
#include <Cobalt/Object.hpp>

namespace Cobalt
{
	struct FieldInfo::Impl
	{
		Impl(const std::string && name, const TypeInfo && type, const FieldInfo::accessor_t & accessor)
			: m_name(std::move(name)),
			m_type(type),
			m_accessor(accessor)
		{
		}

		std::string m_name;
		TypeInfo m_type;
		FieldInfo::accessor_t m_accessor;
	};

	FieldInfo::FieldInfo(const std::string && name, const TypeInfo && type, const accessor_t & accessor)
		: m_pImpl(new Impl(std::move(name), std::move(type), accessor))
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

	bool FieldInfo::operator==(const FieldInfo & field) const
	{
		return m_pImpl->m_name == field.GetName() &&
			m_pImpl->m_type == field.GetType();
	}

	bool FieldInfo::operator==(const FieldInfo && field) const
	{
		return m_pImpl->m_name == field.GetName() &&
			m_pImpl->m_type == field.GetType();
	}

	bool FieldInfo::operator!=(const FieldInfo & field) const
	{
		return m_pImpl->m_name != field.GetName() ||
			m_pImpl->m_type != field.GetType();
	}

	bool FieldInfo::operator!=(const FieldInfo && field) const
	{
		return m_pImpl->m_name != field.GetName() ||
			m_pImpl->m_type != field.GetType();
	}

	FieldInfo & FieldInfo::operator=(const FieldInfo & field)
	{
		m_pImpl = field.m_pImpl;
		return *this;
	}

	FieldInfo & FieldInfo::operator=(const FieldInfo && field) noexcept
	{
		m_pImpl = std::move(field.m_pImpl);
		return *this;
	}
}