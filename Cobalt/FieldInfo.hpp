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
		typedef std::function<Object(const Object &&)> accessor_t;

	public:
		FieldInfo(const std::string & name, const TypeInfo & type, const accessor_t & accessor);
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

		bool operator==(const FieldInfo & field) const;
		bool operator==(const FieldInfo && field) const;
		bool operator!=(const FieldInfo & field) const;
		bool operator!=(const FieldInfo && field) const;
		FieldInfo & operator=(const FieldInfo & field);
		FieldInfo & operator=(const FieldInfo && field) noexcept;

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_FIELD_INFO_HPP_INCLUDED