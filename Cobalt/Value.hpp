#pragma once

#include <Cobalt/TypeInfo.hpp>

#include <memory>
#include <type_traits>

namespace Cobalt
{
	struct Value
	{
	public:
		template <typename T>
		Value(const T & value)
			: m_pImpl(new Impl<T>(value))
		{
		}

		template <typename T>
		Value(const T && value)
			: m_pImpl(new Impl<T>(std::move(value)))
		{
		}

		Value(const Value & value)
			: m_pImpl(value.m_pImpl)
		{
		}

		Value(const Value && value) noexcept
			: m_pImpl(std::move(value.m_pImpl))
		{
		}

		template <typename T>
		operator T() const
		{
			return GetValue<T>();
		}

		Value& operator=(const Value & value)
		{
			m_pImpl = value.m_pImpl;
			return *this;
		}

		Value& operator=(const Value && value) noexcept
		{
			m_pImpl = std::move(value.m_pImpl);
			return *this;
		}

		template <typename T>
		T GetValue() const
		{
			if (m_pImpl->m_type != TypeOf<T>())
				throw std::exception("Can not get value with this type");
			return static_cast<Impl<T>*>(m_pImpl.get())->m_value;
		}

		TypeInfo GetType() const
		{
			return m_pImpl->m_type;
		}

	private:
		struct ImplBase
		{
			ImplBase(const TypeInfo && type)
				: m_type(std::move(type))
			{}
			TypeInfo m_type;
		};
		template <typename T>
		struct Impl : ImplBase
		{
			Impl(const T & value)
				: ImplBase(TypeOf<T>()),
				m_value(value)
			{}
			Impl(const T && value)
				: ImplBase(TypeOf<T>()),
				m_value(std::move(value))
			{}
			T m_value;
		};
		std::shared_ptr<ImplBase> m_pImpl;
	};
}
