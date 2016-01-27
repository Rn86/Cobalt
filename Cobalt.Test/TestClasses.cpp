#include "stdafx.h"
#include <Cobalt.Test/TestClasses.hpp>

#include <Cobalt/Reflection.hpp>

namespace Cobalt
{
	ATestClass::ATestClass(int number)
		: m_number(number)
	{
	}

	void ATestClass::TypeOf(TypeRegistry<ATestClass> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("ATestClass");
		reg.Field(&ATestClass::m_number, "m_number");
	}

	BTestClass::BTestClass()
		: m_member(0)
	{
	}

	BTestClass::BTestClass(ATestClass member)
		: m_member(member)
	{
	}

	ATestClass BTestClass::GetMember() const
	{
		return m_member;
	}

	void BTestClass::SetMember(const ATestClass & member)
	{
		m_member = member;
	}

	void BTestClass::TypeOf(TypeRegistry<BTestClass> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("BTestClass");
		reg.Constructor();
		reg.Constructor<ATestClass>({ "member" });
		reg.Method("GetMember", &BTestClass::GetMember);
		reg.Property("member", &BTestClass::GetMember, &BTestClass::SetMember);
	}
}
