#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	struct OperatorInfo::Impl
	{
		Impl(Operator oper, const MethodInfo & method)
			: m_oper(oper),
			m_method(method)
		{
		}

		Operator m_oper;
		MethodInfo m_method;
	};

	OperatorInfo::OperatorInfo(Operator oper, const MethodInfo & method)
		: m_pImpl(new Impl(oper, method))
	{
	}

	OperatorInfo::OperatorInfo(const OperatorInfo & oper)
		: m_pImpl(oper.m_pImpl)
	{
	}

	OperatorInfo::OperatorInfo(const OperatorInfo && oper) noexcept
		: m_pImpl(std::move(oper.m_pImpl))
	{
	}

	Operator OperatorInfo::GetOperator() const
	{
		return m_pImpl->m_oper;
	}

	MethodInfo OperatorInfo::GetMethod() const
	{
		return m_pImpl->m_method;
	}

	bool OperatorInfo::operator==(const OperatorInfo & oper) const
	{
		return m_pImpl->m_oper == oper.GetOperator() &&
			m_pImpl->m_method == oper.GetMethod();
	}

	bool OperatorInfo::operator==(const OperatorInfo && oper) const
	{
		return m_pImpl->m_oper == oper.GetOperator() &&
			m_pImpl->m_method == oper.GetMethod();
	}

	bool OperatorInfo::operator!=(const OperatorInfo & oper) const
	{
		return m_pImpl->m_oper != oper.GetOperator() ||
			m_pImpl->m_method != oper.GetMethod();
	}

	bool OperatorInfo::operator!=(const OperatorInfo && oper) const
	{
		return m_pImpl->m_oper != oper.GetOperator() ||
			m_pImpl->m_method != oper.GetMethod();
	}

	OperatorInfo & OperatorInfo::operator=(const OperatorInfo & oper)
	{
		m_pImpl = oper.m_pImpl;
		return *this;
	}

	OperatorInfo & OperatorInfo::operator=(const OperatorInfo && oper) noexcept
	{
		m_pImpl = std::move(oper.m_pImpl);
		return *this;
	}

	TypeInfo OperatorInfo::GetType() const
	{
		return Cobalt::TypeOf<OperatorInfo>();
	}

	void OperatorInfo::TypeOf(TypeRegistry<OperatorInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("OperatorInfo");
		reg.Constructor<Operator, const MethodInfo &>({ "oper", "method" });
		reg.Constructor<const OperatorInfo &>({ "oper" });
		reg.Constructor<const OperatorInfo &&>({ "oper" });
		reg.Method("GetOperator", &OperatorInfo::GetOperator);
		reg.Method("GetMethod", &OperatorInfo::GetMethod);
	}
}