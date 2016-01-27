#pragma once

#include <Cobalt/Reflection.hpp>

#include <memory>
#include <type_traits>

namespace Cobalt
{
	template <typename T>
	static TypeInfo TypeOf();

	struct Object
	{
	public:
		Object()
			: m_pImpl(new Impl<void>)
		{
		}

		template <typename T>
		Object(const T & object)
			: m_pImpl(new Impl<T>(object))
		{
		}

		template <typename T>
		Object(const T && object)
			: m_pImpl(new Impl<T>(std::move(object)))
		{
		}

		Object(const Object & object)
			: m_pImpl(object.m_pImpl)
		{
		}

		Object(const Object && object) noexcept
			: m_pImpl(std::move(object.m_pImpl))
		{
		}

		template <typename T>
		operator T() const
		{
			return GetObject<T>();
		}

		Object & operator=(const Object & object)
		{
			m_pImpl = object.m_pImpl;
			return *this;
		}

		Object & operator=(const Object && object) noexcept
		{
			m_pImpl = std::move(object.m_pImpl);
			return *this;
		}

		template <typename T>
		T GetObject() const
		{
			if (m_pImpl->m_type != TypeOf<T>())
				throw std::exception("Types does not match");
			return static_cast<Impl<T>*>(m_pImpl.get())->m_object;
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
			Impl(const T & object)
				: ImplBase(TypeOf<T>()),
				m_object(object)
			{}
			T m_object;
		};
		template <>
		struct Impl<void> : ImplBase
		{
			Impl()
				: ImplBase(TypeOf<void>())
			{
			}
		};
		std::shared_ptr<ImplBase> m_pImpl;
	};
}
