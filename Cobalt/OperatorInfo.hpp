#ifndef COBALT_OPERATOR_INFO_HPP_INCLUDED
#define COBALT_OPERATOR_INFO_HPP_INCLUDED

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct MethodInfo;
	struct Object;

	enum Operator
	{
		addition,
		subtraction,
		equality
	};

	struct OperatorInfo
	{
	public:
		OperatorInfo(Operator oper, const MethodInfo & method);
		OperatorInfo(const OperatorInfo & oper);
		OperatorInfo(const OperatorInfo && oper) noexcept;

		Operator GetOperator() const;
		MethodInfo GetMethod() const;

		template <typename T, typename O, typename ... ARGS>
		T Invoke(const O & object, ARGS ... args) const
		{
			return Invoke(Object(object), { Object(std::forward<ARGS>(args))... }).As<T>();
		}

		Object Invoke(const Object & object, const std::vector<Object> && arguments) const;
		Object Invoke(const Object && object, const std::vector<Object> && arguments) const;

		bool operator==(const OperatorInfo & oper) const;
		bool operator==(const OperatorInfo && oper) const;
		bool operator!=(const OperatorInfo & oper) const;
		bool operator!=(const OperatorInfo && oper) const;
		OperatorInfo & operator=(const OperatorInfo & oper);
		OperatorInfo & operator=(const OperatorInfo && oper) noexcept;

	private:
		friend struct Access;
		static void TypeOf(TypeRegistry<OperatorInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_OPERATOR_INFO_HPP_INCLUDED