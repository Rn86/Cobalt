#include "stdafx.h"
#include <Cobalt.Test/TestClasses.hpp>

#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	ATestClass::ATestClass()
		: m_number(0)
	{
	}

	ATestClass::ATestClass(int number)
		: m_number(number)
	{
	}

	ATestClass::ATestClass(const ATestClass & object)
		: m_number(object.m_number)
	{
	}

	int ATestClass::GetNumber2()
	{
		return 2;
	}

	ATestClass & ATestClass::operator=(const ATestClass & object)
	{
		m_number = object.m_number;
		return *this;
	}

	bool ATestClass::operator==(const ATestClass & object) const
	{
		return m_number == object.m_number;
	}

	TypeInfo ATestClass::GetType() const
	{
		return Cobalt::TypeOf<ATestClass>();
	}

	void ATestClass::TypeOf(TypeRegistry<ATestClass> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("ATestClass");
		reg.Field("m_number", &ATestClass::m_number);
		reg.Method("GetNumber2", &ATestClass::GetNumber2);
		reg.Operator<EQUALITY>({ "object" });
	}

	BTestClass::BTestClass()
		: ATestClass(0), m_member(0)
	{
	}

	BTestClass::BTestClass(ATestClass member)
		: ATestClass(0), m_member(member)
	{
	}

	BTestClass::BTestClass(const BTestClass & object)
		: ATestClass(object), m_member(object.m_member)
	{
	}

	BTestClass & BTestClass::operator=(const BTestClass & object)
	{
		m_member = object.m_member;
		return *this;
	}

	ATestClass BTestClass::GetMember() const
	{
		return m_member;
	}

	void BTestClass::SetMember(const ATestClass & member)
	{
		m_member = member;
	}

	TypeInfo BTestClass::GetType() const
	{
		return Cobalt::TypeOf<BTestClass>();
	}

	void BTestClass::TypeOf(TypeRegistry<BTestClass> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("BTestClass");
		reg.Base<ATestClass>();
		reg.Constructor();
		reg.Constructor<ATestClass>({ "member" });
		reg.Method("GetMember", &BTestClass::GetMember);
		reg.Property("member", &BTestClass::GetMember, &BTestClass::SetMember);
	}
}
