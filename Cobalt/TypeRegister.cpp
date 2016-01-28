#include <Cobalt/TypeRegister.hpp>
#include <Cobalt/TypeInfo.hpp>

#include <map>
#include <mutex>

namespace Cobalt
{
	struct TypeRegisterImpl
	{
		typedef std::map<size_t, TypeInfo> type_map_t;
		typedef std::lock_guard<std::mutex> lock_gurad_t;

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

	TypeInfo TypeRegister::Register(size_t hash, TypeInfo(*typeof)())
	{
		TypeRegisterImpl::lock_gurad_t lock(TypeRegisterImpl::GetInstance().m_mutex);
		TypeRegisterImpl::type_map_t::iterator iter = TypeRegisterImpl::GetInstance().m_typeMap.find(hash);
		if (iter == TypeRegisterImpl::GetInstance().m_typeMap.end())
		{
			TypeInfo type = typeof();
			TypeRegisterImpl::GetInstance().m_typeMap.emplace(hash, type);
			return type;
		}
		return iter->second;
	}
}