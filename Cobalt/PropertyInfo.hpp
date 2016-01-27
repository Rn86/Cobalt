#ifndef COBALT_PROPERTY_INFO_HPP_INCLUDED
#define COBALT_PROPERTY_INFO_HPP_INCLUDED

#include <functional>
#include <vector>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;
	struct MethodInfo;
	struct Object;

	struct PropertyInfo
	{
	public:
		PropertyInfo(const std::string && name, const MethodInfo & getter, const MethodInfo & setter);
		PropertyInfo(const PropertyInfo & property);
		PropertyInfo(const PropertyInfo && property) noexcept;

		std::string GetName() const;
		TypeInfo GetType() const;
		MethodInfo GetGetMethod() const;
		MethodInfo GetSetMethod() const;

		bool operator==(const PropertyInfo & property) const;
		bool operator==(const PropertyInfo && property) const;
		bool operator!=(const PropertyInfo & property) const;
		bool operator!=(const PropertyInfo && property) const;
		PropertyInfo & operator=(const PropertyInfo & property);
		PropertyInfo & operator=(const PropertyInfo && property) noexcept;

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_PROPERTY_INFO_HPP_INCLUDED