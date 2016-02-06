#ifndef COBALT_TEST_CLASSES_HPP_INCLUDED
#define COBALT_TEST_CLASSES_HPP_INCLUDED

#include <Cobalt/Forward.hpp>

namespace Cobalt
{
	class ATestClass
	{
	public:
		ATestClass();
		ATestClass(int number);
		ATestClass(const ATestClass & object);
		ATestClass & operator=(const ATestClass & object);

		bool operator==(const ATestClass & object) const;

		virtual TypeInfo TypeOf() const;

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
		virtual TypeInfo TypeOf() const;
		static void TypeOf(TypeRegistry<BTestClass> & reg);
		ATestClass m_member;
	};
}

#endif //!COBALT_TEST_CLASSES_HPP_INCLUDED