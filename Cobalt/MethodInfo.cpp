#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	struct MethodInfo::Impl
	{
		Impl(const TypeInfo & returnType, const std::string && name, const std::vector<ParameterInfo> & parameters, MethodModifier modifier, const MethodInfo::accessor_t & accessor)
			: m_returnType(returnType),
			m_name(std::move(name)),
			m_parameters(parameters),
			m_modifier(modifier),
			m_accessor(accessor)
		{
		}

		TypeInfo m_returnType;
		std::string m_name;
		std::vector<ParameterInfo> m_parameters;
		MethodModifier m_modifier;
		MethodInfo::accessor_t m_accessor;
	};

	MethodInfo::MethodInfo(const TypeInfo & returnType, const std::string && name, const std::vector<ParameterInfo> & parameters, MethodModifier modifier, const accessor_t & accessor)
		: m_pImpl(new Impl(returnType, std::move(name), parameters, modifier, accessor))
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

	TypeInfo MethodInfo::GetReturnType() const
	{
		return m_pImpl->m_returnType;
	}

	std::string MethodInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	std::vector<ParameterInfo> MethodInfo::GetParameters() const
	{
		return m_pImpl->m_parameters;
	}

	MethodModifier MethodInfo::GetModifier() const
	{
		return m_pImpl->m_modifier;
	}

	Object MethodInfo::Invoke(const Object && object, const std::vector<Object> && arguments) const
	{
		return m_pImpl->m_accessor(std::move(object), std::move(arguments));
	}

	bool MethodInfo::operator==(const MethodInfo & method) const
	{
		return m_pImpl->m_returnType == method.GetReturnType() &&
			m_pImpl->m_name == method.GetName() &&
			m_pImpl->m_parameters == method.GetParameters() &&
			m_pImpl->m_modifier == method.GetModifier();
	}

	bool MethodInfo::operator==(const MethodInfo && method) const
	{
		return m_pImpl->m_returnType == method.GetReturnType() &&
			m_pImpl->m_name == method.GetName() &&
			m_pImpl->m_parameters == method.GetParameters() &&
			m_pImpl->m_modifier == method.GetModifier();
	}

	bool MethodInfo::operator!=(const MethodInfo & method) const
	{
		return m_pImpl->m_returnType != method.GetReturnType() ||
			m_pImpl->m_name != method.GetName() ||
			m_pImpl->m_parameters != method.GetParameters() ||
			m_pImpl->m_modifier != method.GetModifier();
	}

	bool MethodInfo::operator!=(const MethodInfo && method) const
	{
		return m_pImpl->m_returnType != method.GetReturnType() ||
			m_pImpl->m_name != method.GetName() ||
			m_pImpl->m_parameters != method.GetParameters() ||
			m_pImpl->m_modifier != method.GetModifier();
	}

	MethodInfo & MethodInfo::operator=(const MethodInfo & method)
	{
		m_pImpl = method.m_pImpl;
		return *this;
	}

	MethodInfo & MethodInfo::operator=(const MethodInfo && method) noexcept
	{
		m_pImpl = std::move(method.m_pImpl);
		return *this;
	}

	TypeInfo MethodInfo::GetType() const
	{
		return Cobalt::TypeOf<MethodInfo>();
	}

	void MethodInfo::TypeOf(TypeRegistry<MethodInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("MethodInfo");
		reg.Constructor<const TypeInfo &, const std::string &&, const std::vector<ParameterInfo> &, MethodModifier, const accessor_t &>({ "returnType", "name", "parameters", "modifier", "accessor" });
		reg.Constructor<const MethodInfo &>({ "method" });
		reg.Constructor<const MethodInfo &&>({ "method" });
		reg.Method("GetReturnType", &MethodInfo::GetReturnType);
		reg.Method("GetName", &MethodInfo::GetName);
		reg.Method("GetParameters", &MethodInfo::GetParameters);
		reg.Method("GetModifier", &MethodInfo::GetModifier);
		reg.Method("Invoke", &MethodInfo::Invoke);
	}
}