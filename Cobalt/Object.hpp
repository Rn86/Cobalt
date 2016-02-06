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

		Object(nullptr_t)
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
			return As<T>();
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

		Object operator+(const Object & object) const
		{
			auto method = m_pImpl->m_type.GetOperator(Operator::ADDITION).GetMethod();
			return method.Invoke(Object(*this), std::vector<Object>{ object });
		}

		Object operator+(const Object && object) const
		{
			auto method = m_pImpl->m_type.GetOperator(Operator::ADDITION).GetMethod();
			return method.Invoke(Object(*this), std::vector<Object>{ std::move(object) });
		}

		bool operator==(const Object & object) const
		{
			auto method = m_pImpl->m_type.GetOperator(Operator::EQUALITY).GetMethod();
			return method.Invoke(Object(*this), std::vector<Object>{ object });
		}

		bool operator==(const Object && object) const
		{
			auto method = m_pImpl->m_type.GetOperator(Operator::EQUALITY).GetMethod();
			return method.Invoke(Object(*this), std::vector<Object>{ std::move(object) });
		}

		bool Is(const TypeInfo & type) const
		{
			return m_pImpl->m_type == type;
		}

		bool Is(const TypeInfo && type) const
		{
			return m_pImpl->m_type == std::move(type);
		}

		template <typename T>
		T As() const
		{
			auto type = TypeOf<T>();
			if (m_pImpl->m_type == TypeOf<T>() || ((std::is_reference<T>::value || std::is_pointer<T>::value) && type.IsSubclassOf(m_pImpl->m_type)))
				return std::move(static_cast<Impl<T>*>(m_pImpl.get())->m_object);
			throw std::exception("Can not cast to target type");
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

	static const Object null{};
}
