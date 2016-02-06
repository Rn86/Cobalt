#ifndef COBALT_TYPE_INFO_HPP_INCLUDED
#define COBALT_TYPE_INFO_HPP_INCLUDED

#include <Cobalt/Forward.hpp>

#include <Cobalt/ConstructorInfo.hpp>
#include <Cobalt/FieldInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/MethodInfo.hpp>
#include <Cobalt/PropertyInfo.hpp>
#include <Cobalt/OperatorInfo.hpp>

#include <string>
#include <memory>

namespace Cobalt
{
	struct TypeInfo
	{
	public:
		TypeInfo(const struct TypeInfoParameters & parameters);
		TypeInfo(const TypeInfo & info);
		TypeInfo(const TypeInfo && info) noexcept;

		size_t GetHashCode() const;
		std::string GetNamespace() const;
		std::string GetName() const;
		size_t GetSize() const;
		std::vector<FieldInfo> GetFields() const;
		std::vector<ConstructorInfo> GetConstructors() const;
		std::vector<MethodInfo> GetMethods() const;
		std::vector<PropertyInfo> GetProperties() const;
		std::vector<TypeInfo> GetBaseTypes() const;
		std::vector<OperatorInfo> GetOperators() const;

		bool IsSubclassOf(const TypeInfo & type) const;
		bool IsSubclassOf(const TypeInfo && type) const;

		ConstructorInfo GetConstructor(const std::vector<TypeInfo> && types) const;
		OperatorInfo GetOperator(Operator oper) const;

		template <typename ... ARGS>
		ConstructorInfo GetConstructor() const
		{
			return GetConstructor({ TypeOf<ARGS>()... });
		}

		template <typename T>
		bool IsSubclassOf() const
		{
			return IsSubclassOf(TypeOf<T>());
		}

		bool operator==(const TypeInfo & type) const;
		bool operator==(const TypeInfo && type) const;
		bool operator!=(const TypeInfo & type) const;
		bool operator!=(const TypeInfo && type) const;
		TypeInfo & operator=(const TypeInfo & type);
		TypeInfo & operator=(const TypeInfo && type) noexcept;

	private:
		friend struct Access;
		friend struct TypeRegister;
		virtual TypeInfo GetType() const;
		static void TypeOf(TypeRegistry<TypeInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#include <Cobalt/Object.hpp>

#endif //!COBALT_TYPE_INFO_HPP_INCLUDED