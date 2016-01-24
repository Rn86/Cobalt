#ifndef COBALT_FIELD_INFO_HPP_INCLUDED
#define COBALT_FIELD_INFO_HPP_INCLUDED

#include <functional>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct Value;

	struct FieldInfo
	{
	public:
		FieldInfo(const std::string & name, const TypeInfo & type, const std::function<Value(const Value &&)> & accessor);
		FieldInfo(const FieldInfo & field);
		FieldInfo(const FieldInfo && field) noexcept;

		std::string GetName() const;
		TypeInfo GetType() const;

		template <typename T, typename O>
		T Invoke(const O & object) const
		{
			return Invoke(Value(object)).GetValue<T>();
		}

		Value Invoke(const Value && value) const;

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_FIELD_INFO_HPP_INCLUDED