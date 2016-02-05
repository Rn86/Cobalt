#ifndef COBALT_TEST_CLASSES_HPP_INCLUDED
#define COBALT_TEST_CLASSES_HPP_INCLUDED

#include <Cobalt/Forward.hpp>
#include <Cobalt/Reflectable.hpp>

namespace Cobalt
{
	class ATestClass : Reflectable<ATestClass>
	{
	public:
		ATestClass();
		ATestClass(int number);
		ATestClass(const ATestClass & object);
		ATestClass & operator=(const ATestClass & object);

		bool operator==(const ATestClass & object) const;

		int m_number;
	private:
		friend Access;
		static void TypeOf(TypeRegistry<ATestClass> & reg);
	};

	class BTestClass : public ATestClass
	{
	public:
		BTestClass();
		BTestClass(const BTestClass & object);
		BTestClass(ATestClass member);
		BTestClass & operator=(const BTestClass & object);

		ATestClass GetMember() const;
		void SetMember(const ATestClass & member);

	private:
		friend Access;
		static void TypeOf(TypeRegistry<BTestClass> & reg);
		ATestClass m_member;
	};
}

#endif //!COBALT_TEST_CLASSES_HPP_INCLUDED