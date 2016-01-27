#ifndef COBALT_TEST_CLASSES_HPP_INCLUDED
#define COBALT_TEST_CLASSES_HPP_INCLUDED

#include <Cobalt/Forward.hpp>

namespace Cobalt
{
	class ATestClass
	{
	public:
		ATestClass(int number);
		int m_number;
	private:
		friend struct Access;
		static void TypeOf(TypeRegistry<ATestClass> & reg);
	};

	class BTestClass
	{
	public:
		BTestClass();
		BTestClass(ATestClass member);

		ATestClass GetMember() const;
		void SetMember(const ATestClass & member);

	private:
		friend struct Access;
		static void TypeOf(TypeRegistry<BTestClass> & reg);
		ATestClass m_member;
	};
}

#endif //!COBALT_TEST_CLASSES_HPP_INCLUDED