#include "stdafx.h"
#include "CppUnitTest.h"

#include <Cobalt.Test/TestClasses.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <Cobalt/Activator.hpp>
#include <Cobalt/TypeBuilder.hpp>

namespace Cobalt
{
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

		TEST_METHOD(ValueTest)
		{
			ATestClass expected(13);
			auto value = Object(expected);
			ATestClass actual = value.As<ATestClass>();
			Assert::IsTrue(expected.m_number == actual.m_number);
		}

		TEST_METHOD(FieldTest)
		{
			int expected = 17;
			ATestClass a(expected);
			auto info = TypeOf(a);
			auto field = info.GetFields()[0];
			int actual = field.Invoke(a);
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ConstructorTest)
		{
			auto info = TypeOf<BTestClass>();
			auto constructor = info.GetConstructor({ TypeOf<ATestClass>() });
			int expected = 21;
			ATestClass a(expected);
			auto b = constructor.Invoke<BTestClass>(a);
			int actual = b.GetMember().m_number;
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
			int actual = b.GetMember().m_number;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(ActivatorTest2)
		{
			int expected = 23;
			ATestClass a(expected);
			BTestClass b = Activator::CreateInstance(TypeOf<BTestClass>(), a);
			int actual = b.GetMember().m_number;
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
			int actual = aa.m_number;
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
			int actual = aa.m_number;
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(GetTypeOfTest)
		{
			ATestClass a(26);
			ATestClass * pa = new BTestClass(a);
			TypeInfo type = TypeOf(pa);
			std::string expected = "BTestClass";
			std::string actual = type.GetName();
			Logger::WriteMessage(actual.c_str());
			Assert::IsTrue(expected == actual);
		}

		TEST_METHOD(OperatorDispatchTest)
		{
			ATestClass a(26);
			ATestClass b(26);
			Object _a(a);
			Object _b(b);
			Assert::IsTrue(_a == _b);
		}

		TEST_METHOD(TypeBuilderTest1)
		{
			TypeBuilder builder("Cobalt", "Runtime_Type");
			builder.DefineField("m_field", std::function<int(const ATestClass &)>([](const ATestClass & object)
			{
				return object.m_number;
			}));
			auto type = builder.CreateType();
			auto fields = type.GetFields();
			int expected = 1;
			int actual = fields.size();
			Assert::IsTrue(expected == actual);
		}
	};

}