#include <Cobalt/Reflection.hpp>

#include <map>
#include <mutex>

namespace Cobalt
{
	struct TypeInfo::Impl
	{
		Impl(const TypeInfoParameters & reg)
			: m_hash(reg.m_hash),
			m_space(reg.m_space),
			m_name(reg.m_name),
			m_size(reg.m_size),
			m_fields(reg.m_fields),
			m_constructors(reg.m_constructors),
			m_methods(reg.m_methods),
			m_properties(reg.m_properties),
			m_bases(reg.m_bases),
			m_operators(reg.m_operators)
		{
		}

		size_t m_hash;
		std::string m_name;
		std::string m_space;
		size_t m_size;
		std::vector<FieldInfo> m_fields;
		std::vector<ConstructorInfo> m_constructors;
		std::vector<MethodInfo> m_methods;
		std::vector<PropertyInfo> m_properties;
		std::vector<TypeInfo> m_bases;
		std::vector<OperatorInfo> m_operators;
	};


	struct TypeRegisterImpl
	{
		typedef std::map<size_t, TypeInfo> type_map_t;
		typedef std::lock_guard<std::mutex> lock_guard_t;

		TypeRegisterImpl()
		{
		}

		static TypeRegisterImpl & GetInstance()
		{
			static TypeRegisterImpl instance;
			return instance;
		}

		std::map<size_t, TypeInfo> m_typeMap;
		std::mutex m_mutex;
	};

	TypeInfo TypeRegister::Register(size_t hash, const std::function<TypeInfo()> & typeof)
	{
		TypeRegisterImpl::type_map_t::iterator iter = TypeRegisterImpl::GetInstance().m_typeMap.end();
		{
			TypeRegisterImpl::lock_guard_t lock(TypeRegisterImpl::GetInstance().m_mutex);
			iter = TypeRegisterImpl::GetInstance().m_typeMap.find(hash);
		}
		if (iter == TypeRegisterImpl::GetInstance().m_typeMap.end())
		{
			TypeInfoParameters parameters;
			parameters.m_hash = hash;
			TypeInfo proxy(parameters);
			{
				TypeRegisterImpl::lock_guard_t lock(TypeRegisterImpl::GetInstance().m_mutex);
				TypeRegisterImpl::GetInstance().m_typeMap.emplace(hash, proxy);
			}
			TypeInfo type = typeof();
			{
				TypeRegisterImpl::lock_guard_t lock(TypeRegisterImpl::GetInstance().m_mutex);
				proxy.m_pImpl->m_bases = type.GetBaseTypes();
				proxy.m_pImpl->m_constructors = type.GetConstructors();
				proxy.m_pImpl->m_fields = type.GetFields();
				proxy.m_pImpl->m_methods = type.GetMethods();
				proxy.m_pImpl->m_name = type.GetName();
				proxy.m_pImpl->m_operators = type.GetOperators();
				proxy.m_pImpl->m_properties = type.GetProperties();
				proxy.m_pImpl->m_size = type.GetSize();
				proxy.m_pImpl->m_space = type.GetNamespace();
			}
			return proxy;
		}
		return iter->second;
	}


	TypeInfo::TypeInfo(const TypeInfoParameters & reg)
		: m_pImpl(new Impl(reg))
	{
	}

	TypeInfo::TypeInfo(const TypeInfo & info)
		: m_pImpl(info.m_pImpl)
	{
	}

	TypeInfo::TypeInfo(const TypeInfo && info) noexcept
		: m_pImpl(std::move(info.m_pImpl))
	{
	}

	size_t TypeInfo::GetHashCode() const
	{
		return m_pImpl->m_hash;
	}

	std::string TypeInfo::GetName() const
	{
		return m_pImpl->m_name;
	}

	std::string TypeInfo::GetNamespace() const
	{
		return m_pImpl->m_space;
	}

	size_t TypeInfo::GetSize() const
	{
		return m_pImpl->m_size;
	}

	std::vector<FieldInfo> TypeInfo::GetFields() const
	{
		return m_pImpl->m_fields;
	}

	std::vector<ConstructorInfo> TypeInfo::GetConstructors() const
	{
		return m_pImpl->m_constructors;
	}

	std::vector<MethodInfo> TypeInfo::GetMethods() const
	{
		return m_pImpl->m_methods;
	}

	std::vector<PropertyInfo> TypeInfo::GetProperties() const
	{
		return m_pImpl->m_properties;
	}

	std::vector<TypeInfo> TypeInfo::GetBaseTypes() const
	{
		return m_pImpl->m_bases;
	}

	std::vector<OperatorInfo> TypeInfo::GetOperators() const
	{
		return m_pImpl->m_operators;
	}

	bool TypeInfo::IsSubclassOf(const TypeInfo & type) const
	{
		bool subclass = false;
		for (auto & base : m_pImpl->m_bases)
		{
			subclass = base == type;
			if (subclass) break;
			subclass = base.IsSubclassOf(type);
			if (subclass) break;
		}
		return subclass;
	}

	bool TypeInfo::IsSubclassOf(const TypeInfo && type) const
	{
		bool subclass = false;
		for (auto & base : m_pImpl->m_bases)
		{
			subclass = base == std::move(type);
			if (subclass) break;
			subclass = base.IsSubclassOf(std::move(type));
			if (subclass) break;
		}
		return subclass;
	}

	ConstructorInfo TypeInfo::GetConstructor(const std::vector<TypeInfo> && types) const
	{
		bool found = false;
		for (auto & constructor : m_pImpl->m_constructors)
		{
			auto parameters = constructor.GetParameters();
			for (size_t i = 0; i < parameters.size(); i++)
			{
				found = (types[i] == parameters[i].GetParameterType());
				if (!found) break;
			}
			if (found) return constructor;
		}
		throw std::exception("Could not find appropriet constructor");
	}

	MethodInfo TypeInfo::GetMethod(const std::string && name) const
	{
		for (auto & method : m_pImpl->m_methods)
		{
			if (name == method.GetName())
				return method;
		}
		throw std::exception("Could not find appropriet method");
	}

	OperatorInfo TypeInfo::GetOperator(Operator oper) const
	{
		for (auto & operat : m_pImpl->m_operators)
		{
			if (oper == operat.GetOperator())
				return operat;
		}
		throw std::exception("Could not find appropriet operator");
	}

	bool TypeInfo::operator==(const TypeInfo & type) const
	{
		return type.GetHashCode() == GetHashCode();
	}

	bool TypeInfo::operator==(const TypeInfo && type) const
	{
		return type.GetHashCode() == GetHashCode();
	}

	bool TypeInfo::operator!=(const TypeInfo & type) const
	{
		return type.GetHashCode() != GetHashCode();
	}

	bool TypeInfo::operator!=(const TypeInfo && type) const
	{
		return type.GetHashCode() != GetHashCode();
	}

	TypeInfo & TypeInfo::operator=(const TypeInfo & type)
	{
		m_pImpl = type.m_pImpl;
		return *this;
	}

	TypeInfo & TypeInfo::operator=(const TypeInfo && type) noexcept
	{
		m_pImpl = std::move(type.m_pImpl);
		return *this;
	}

	TypeInfo TypeInfo::GetType() const
	{
		return Cobalt::TypeOf<TypeInfo>();
	}

	void TypeInfo::TypeOf(TypeRegistry<TypeInfo> & reg)
	{
		reg.Namespace("Cobalt");
		reg.Name("TypeInfo");
		reg.Constructor<const TypeInfo &>({ "type" });
		reg.Constructor<const TypeInfo &&>({ "type" });
		reg.Method("GetHashCode", &TypeInfo::GetHashCode);
		reg.Method("GetNamespace", &TypeInfo::GetNamespace);
		reg.Method("GetName", &TypeInfo::GetName);
		reg.Method("GetFields", &TypeInfo::GetFields);
		reg.Method("GetConstructors", &TypeInfo::GetConstructors);
		reg.Method("GetMethods", &TypeInfo::GetMethods);
		reg.Method("GetProperties", &TypeInfo::GetProperties);
		reg.Method("GetBaseTypes", &TypeInfo::GetBaseTypes);
		reg.Method("GetOperators", &TypeInfo::GetOperators);
	}
}