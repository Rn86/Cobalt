#ifndef COBALT_FIELD_INFO_HPP_INCLUDED
#define COBALT_FIELD_INFO_HPP_INCLUDED

#include <functional>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct Object;

	struct FieldInfo
	{
	public:
		FieldInfo(const std::string & name, const TypeInfo & type, const std::function<Object(const Object &&)> & accessor);
		FieldInfo(const FieldInfo & field);
		FieldInfo(const FieldInfo && field) noexcept;

		std::string GetName() const;
		TypeInfo GetType() const;

		template <typename T, typename O>
		T Invoke(const O & object) const
		{
			return Invoke(Object(object)).GetObject<T>();
		}

		Object Invoke(const Object && object) const;

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_FIELD_INFO_HPP_INCLUDED