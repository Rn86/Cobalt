#ifndef COBALT_PARAMETER_INFO_HPP_INCLUDED
#define COBALT_PARAMETER_INFO_HPP_INCLUDED

#include <string>
#include <memory>

namespace Cobalt
{
	struct TypeInfo;

	struct ParameterInfo
	{
	public:
		ParameterInfo(const std::string && name, const TypeInfo & type);
		ParameterInfo(const ParameterInfo & parameter);
		ParameterInfo(const ParameterInfo && parameter) noexcept;

		std::string GetName() const;
		TypeInfo GetParameterType() const;

		bool operator==(const ParameterInfo & parameter) const;
		bool operator==(const ParameterInfo && parameter) const;
		bool operator!=(const ParameterInfo & parameter) const;
		bool operator!=(const ParameterInfo && parameter) const;
		ParameterInfo & operator=(const ParameterInfo & parameter);
		ParameterInfo & operator=(const ParameterInfo && parameter) noexcept;

	private:
		friend struct Access;
		virtual TypeInfo GetType() const;
		static void TypeOf(TypeRegistry<ParameterInfo> & reg);

	private:
		struct Impl;
		std::shared_ptr<Impl> m_pImpl;
	};
}

#endif //!COBALT_PARAMETER_INFO_HPP_INCLUDED
