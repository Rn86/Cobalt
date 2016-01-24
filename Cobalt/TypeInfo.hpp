#ifndef COBALT_TYPE_INFO_HPP_INCLUDED
#define COBALT_TYPE_INFO_HPP_INCLUDED

#include <Cobalt/ConstructorInfo.hpp>
#include <Cobalt/FieldInfo.hpp>
#include <Cobalt/ParameterInfo.hpp>
#include <Cobalt/MethodInfo.hpp>

#include <string>
#include <memory>

namespace Cobalt
{
	template <typename T>
	struct TypeRegistry;
	struct TypeInfoParameters;

	struct TypeInfo
	{
	public:
		TypeInfo(const TypeInfoParameters & parameters);
		TypeInfo(const TypeInfo & info);
		TypeInfo(const TypeInfo && info) noexcept;

		size_t GetHashCode() const;
		std::string GetNamespace() const;
		std::string GetName() const;
		std::vector<FieldInfo> GetFields() const;
		std::vector<ConstructorInfo> GetConstructors() const;
		std::vector<MethodInfo> GetMethods() const;

		template <typename ... ARGS>
		ConstructorInfo GetConstructor() const
		{
			return GetConstructor({ TypeOf<ARGS>()... });
		}

		ConstructorInfo GetConstructor(const std::vector<TypeInfo> && types) const;

		operator std::string() const;
		bool operator==(const TypeInfo & type) const;
		bool operator==(const TypeInfo && type) const;
		bool operator!=(const TypeInfo & type) const;
		bool operator!=(const TypeInfo && type) const;

	private:
		friend struct Access;
		static void TypeOf(TypeRegistry<TypeInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#include <Cobalt/Value.hpp>

#endif //!COBALT_TYPE_INFO_HPP_INCLUDED