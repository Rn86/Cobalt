#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <Cobalt/Reflection.hpp>
#include <Cobalt/Activator.hpp>

namespace Cobalt
{

	class ATestClass
	{
	public:
		ATestClass(int number)
			: m_number(number)
		{
		}

		int GetNumber() const
		{
			return m_number;
		}

		int m_number;

	private:
		friend struct Access;

		static void TypeOf(TypeRegistry<ATestClass> & reg)
		{
			reg.Namespace("Cobalt");
			reg.Name("ATestClass");
		}

		
	};

	class BTestClass
	{
	public:
		BTestClass()
			: m_member(0),
			m_other(0)
		{
		}

		BTestClass(ATestClass member)
			: m_member(member),
			m_other(17)
		{
		}

		ATestClass GetMember() const
		{
			return m_member;
		}

		void SetMember(const ATestClass & member)
		{
			m_member = member;
		}

		ATestClass m_member;

	private:
		friend struct Access;

		static void TypeOf(TypeRegistry<BTestClass> & reg)
		{
			reg.Namespace("Cobalt");
			reg.Name("BTestClass");
			reg.Field(&BTestClass::m_member, "m_member");
			reg.Constructor();
			reg.Constructor<ATestClass>({ "member" });
			reg.Field(&BTestClass::m_other, "m_other");
			reg.Method("GetMember", &BTestClass::GetMember);
			reg.Property("member", &BTestClass::GetMember, &BTestClass::SetMember);
		}

		ATestClass m_other;
	};


	TEST_CLASS(UnitTest1)
	{
	public:


		TEST_METHOD(TestMethod1)
		{
			auto info = TypeOf<ATestClass>();
			std::string expected = "ATestClass";
			std::string actual = info.GetName();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(TestMethod2)
		{
			ATestClass a(13);
			auto info = TypeOf(a);
			std::string expected = "ATestClass";
			std::string actual = info.GetName();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(TestMethod3)
		{
			auto info = TypeOf<ATestClass>();
			std::string expected = "Cobalt";
			std::string actual = info.GetNamespace();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(TestMethod4)
		{
			auto info = TypeOf<ATestClass>();
			std::string expected = "Cobalt::ATestClass";
			std::string actual = info;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ValueTest)
		{
			ATestClass expected(13);
			auto value = Object(expected);
			ATestClass actual = value.GetObject<ATestClass>();
			Assert::IsTrue(expected.m_number == actual.m_number);
		}

		TEST_METHOD(FieldTest)
		{
			int expected = 17;
			BTestClass b(expected);
			auto info = TypeOf(b);
			auto field = info.GetFields()[0];
			ATestClass a = field.Invoke<ATestClass>(b);
			int actual = a.GetNumber();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(FieldTest2)
		{
			int expected = 17;
			BTestClass b(expected);
			auto info = TypeOf(b);
			auto field = info.GetFields()[1];
			ATestClass a = field.Invoke<ATestClass>(b);
			int actual = a.GetNumber();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ConstructorTest)
		{
			auto info = TypeOf<BTestClass>();
			auto constructor = info.GetConstructor({ TypeOf<ATestClass>() });
			int expected = 21;
			ATestClass a(expected);
			auto b = constructor.Invoke<BTestClass>(a);
			int actual = b.m_member.m_number;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ConstructorParameterTest)
		{
			auto info = TypeOf<BTestClass>();
			auto constructor = info.GetConstructor({ TypeOf<ATestClass>() });
			auto parameter = constructor.GetParameters()[0];
			std::string expected = "member";
			std::string actual = parameter.GetName();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ActivatorTest)
		{
			int expected = 23;
			ATestClass a(expected);
			auto b = Activator::CreateInstance<BTestClass>(a);
			int actual = b.m_member.m_number;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ActivatorTest2)
		{
			int expected = 23;
			ATestClass a(expected);
			BTestClass b = Activator::CreateInstance(TypeOf<BTestClass>(), a);
			int actual = b.m_member.m_number;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(MethodTest1)
		{
			int expected = 26;
			ATestClass a(expected);
			BTestClass b(a);
			TypeInfo type = TypeOf(b);
			MethodInfo method = type.GetMethods()[0];
			ATestClass aa = method.Invoke<ATestClass>(b);
			int actual = aa.GetNumber();
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(PropertyTest)
		{
			int expected = 26;
			ATestClass a(expected);
			BTestClass b(a);
			TypeInfo type = TypeOf(b);
			PropertyInfo property = type.GetProperties()[0];
			MethodInfo method = property.GetGetMethod();
			ATestClass aa = method.Invoke<ATestClass>(b);
			int actual = aa.GetNumber();
			Assert::IsTrue(expected == actual);
		}
	};

}