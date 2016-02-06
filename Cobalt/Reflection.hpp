#ifndef COBALT_REFLECTION_HPP_INCLUDED
#define COBALT_REFLECTION_HPP_INCLUDED

#include <Cobalt/TypeInfo.hpp>

#include <string>
#include <vector>
#include <functional>

namespace Cobalt
{
	struct TypeInfoParameters
	{
		size_t m_hash;
		std::string m_space;
		std::string m_name;
		size_t m_size;
		std::vector<FieldInfo> m_fields;
		std::vector<ConstructorInfo> m_constructors;
		std::vector<MethodInfo> m_methods;
		std::vector<PropertyInfo> m_properties;
		std::vector<TypeInfo> m_bases;
		std::vector<OperatorInfo> m_operators;
	};

	template <typename T>
	struct TypeRegistry
	{
	public:
		TypeRegistry()
		{
			m_parameters.m_hash = typeid(T).hash_code();
			m_parameters.m_name = typeid(T).name();
			m_parameters.m_size = sizeof(T);
		}

		void Namespace(const std::string && space)
		{
			m_parameters.m_space = space;
		}

		void Name(const std::string && name)
		{
			m_parameters.m_name = name;
		}

		template <typename F>
		void Field(const std::string && name, F (T::*field))
		{
			auto accessor = [field](const Object && object)
			{
				auto obj = object.As<T>();
				return Object(obj.*field);
			};
			m_parameters.m_fields.push_back(FieldInfo(std::move(name), TypeOf<F>(), accessor));
		}

		template<int ...>
		struct sequence { };
		template<int N, int ...S>
		struct gen_sequence : gen_sequence<N - 1, N - 1, S...> { };
		template<int ...S>
		struct gen_sequence<0, S...>
		{
			typedef sequence<S...> type;
		};

		template <typename ... ARGS>
		struct ConstructorMaker
		{
			template<int ... S>
			static Object Make(const std::vector<Object> && arguments, sequence<S...>)
			{
				return Object(T(std::forward<ARGS>(arguments[S].As<ARGS>())...));
			}
		};

		template <typename ... ARGS>
		struct ParametersMaker
		{
			template<int ... S>
			static std::vector<ParameterInfo> Make(const std::vector<std::string> && names, sequence<S...>)
			{
				return{ ParameterInfo(std::move(names[S]), TypeOf<ARGS>())... };
			}
		};

		void Constructor()
		{
			auto accessor = [](const std::vector<Object> && arguments)
			{
				if (arguments.size() > 0)
					throw std::exception("Object can not be constructed");
				return Object(T());
			};
			m_parameters.m_constructors.push_back(ConstructorInfo({}, accessor));
		}

		template <typename ... ARGS>
		void Constructor(const std::vector<std::string> && names)
		{
			auto accessor = [](const std::vector<Object> && arguments)
			{
				return ConstructorMaker<ARGS...>::Make(std::move(arguments), typename gen_sequence<sizeof...(ARGS)>::type()).As<T>();
			};
			std::vector<ParameterInfo> parameters = ParametersMaker<ARGS...>::Make(std::move(names), typename gen_sequence<sizeof...(ARGS)>::type());
			m_parameters.m_constructors.push_back(ConstructorInfo(parameters, accessor));
		}

		template <bool stat, typename R, typename... ARGS>
		struct MethodMakerBaseEx
		{
			template <typename M, int ... S>
			static Object Make(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				return Object((object.As<T>().*method)(std::forward<ARGS>(arguments[S].As<ARGS>())...));
			}

			template <typename M>
			static std::function<Object(const Object &&, const std::vector<Object> &&)> Make(M method)
			{
				return[method](const Object && object, const std::vector<Object> && arguments)
				{
					return MethodMakerBase<stat, R, ARGS...>::MakeEx(std::move(object), method, std::move(arguments), typename gen_sequence<sizeof...(ARGS)>::type());
				};
			}

			static std::vector<ParameterInfo> MakeParameters(const std::vector<std::string> && names)
			{
				return ParametersMaker<ARGS...>::Make(std::move(names), typename gen_sequence<sizeof...(ARGS)>::type());
			}


			static TypeInfo GetReturnType()
			{
				return TypeOf<R>();
			}
		};

		template <bool stat, typename R, typename... ARGS>
		struct MethodMakerBase : MethodMakerBaseEx<stat, R, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				return Object((object.As<T>().*method)(std::forward<ARGS>(arguments[S].As<ARGS>())...));
			}
		};
		template <bool stat, typename... ARGS>
		struct MethodMakerBase<stat, void, ARGS...> : MethodMakerBaseEx<stat, void, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				(object.As<T>().*method)(std::forward<ARGS>(arguments[S].As<ARGS>())...);
				return Object();
			}
		};
		template <typename R, typename... ARGS>
		struct MethodMakerBase<true, R, ARGS...> : MethodMakerBaseEx<true, R, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object &&, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				return Object(method(std::forward<ARGS>(arguments[S].As<ARGS>())...));
			}
		};
		template <typename... ARGS>
		struct MethodMakerBase<true, void, ARGS...> : MethodMakerBaseEx<true, void, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object &&, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				method(std::forward<ARGS>(arguments[S].As<ARGS>())...);
				return Object();
			}
		};

		template <typename M>
		struct MethodMaker;
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...)> : MethodMakerBase<false, R, ARGS...>
		{
			static constexpr bool IsStatic = false;
			static constexpr MethodModifier modifier = mmNone;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) const> : MethodMakerBase<false, R, ARGS...>
		{
			static constexpr bool IsStatic = false;
			static constexpr MethodModifier modifier = mmConst;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) volatile> : MethodMakerBase<false, R, ARGS...>
		{
			static constexpr bool IsStatic = false;
			static constexpr MethodModifier modifier = mmVolatile;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) const volatile> : MethodMakerBase<false, R, ARGS...>
		{
			static constexpr bool IsStatic = false;
			static constexpr MethodModifier modifier = mmConst | mmVolatile;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(*)(ARGS...)> : MethodMakerBase<true, R, ARGS...>
		{
			static constexpr bool IsStatic = true;
			static constexpr MethodModifier modifier = mmNone;
		};

		template <typename M>
		void Method(const std::string && name, M method, const std::vector<std::string> && names)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), MethodMaker<M>::MakeParameters(std::move(names)), MethodMaker<M>::modifier, MethodMaker<M>::IsStatic, MethodMaker<M>::Make(method)));
		}

		template <typename M>
		void Method(const std::string && name, M method)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), {}, MethodMaker<M>::modifier, MethodMaker<M>::IsStatic, MethodMaker<M>::Make(method)));
		}

		template <typename G, typename S>
		void Property(const std::string && name, G getter, S setter)
		{
			m_parameters.m_properties.push_back(PropertyInfo(
				std::move(name),
				MethodInfo(MethodMaker<G>::GetReturnType(), "get_" + name, {}, MethodMaker<G>::modifier, false, MethodMaker<G>::Make(getter)),
				MethodInfo(MethodMaker<S>::GetReturnType(), "set_" + name, MethodMaker<S>::MakeParameters({ std::move(name) }), MethodMaker<S>::modifier, false, MethodMaker<S>::Make(setter))
				));
		}

		template <typename T>
		void Base()
		{
			m_parameters.m_bases.push_back(TypeOf<T>());
		}

		template <typename M>
		static MethodInfo MakeMethod(const std::string && name, M method, const std::vector<std::string> && names)
		{
			return MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), MethodMaker<M>::MakeParameters(std::move(names)), MethodMaker<M>::modifier, MethodMaker<M>::IsStatic, MethodMaker<M>::Make(method));
		}

		template <Cobalt::Operator oper>
		struct OperatorMaker;
		template <>
		struct OperatorMaker<Cobalt::Operator::ADDITION>
		{
			static OperatorInfo Make(const std::vector<std::string> && names)
			{
				return{ Cobalt::Operator::ADDITION, MakeMethod("operator+", &T::operator+, std::move(names)) };
			}
		};
		template <>
		struct OperatorMaker<Cobalt::Operator::SUBTRACTION>
		{
			static OperatorInfo Make(const std::vector<std::string> && names)
			{
				return{ Cobalt::Operator::SUBTRACTION, MakeMethod("operator-", &T::operator-, std::move(names)) };
			}
		};
		template <>
		struct OperatorMaker<Cobalt::Operator::EQUALITY>
		{
			static OperatorInfo Make(const std::vector<std::string> && names)
			{
				return{ Cobalt::Operator::EQUALITY, MakeMethod("operator==", &T::operator==, std::move(names)) };
			}
		};

		template <Cobalt::Operator oper>
		void Operator(const std::vector<std::string> && names)
		{
			m_parameters.m_operators.push_back(OperatorMaker<oper>::Make(std::move(names)));
		}

		template <Cobalt::Operator oper>
		void Operator()
		{
			m_parameters.m_operators.push_back(OperatorMaker<oper>::Make({}));
		}

		TypeInfoParameters m_parameters;
	};

	struct Access
	{
	private:
		friend struct TypeOfAccess;
		template <typename T>
		friend struct is_staticly_reflectable;
		template <typename T>
		friend struct is_dynamically_reflectable;

		template <typename T>
		static void TypeOf(TypeRegistry<T> & reg)
		{
			T::TypeOf(reg);
		}

		template <typename T, bool ptr>
		struct CallGetType
		{
			static auto GetType(const T & object)
			{
				return object.GetType();
			}
		};
		template <typename T>
		struct CallGetType<T, true>
		{
			static auto GetType(const T & object)
			{
				return object->GetType();
			}
		};

		template <typename T>
		static auto GetType(const T & object)
		{
			return CallGetType<T, std::is_pointer<T>::value>::GetType(object);
		}

		template <typename T>
		struct is_staticly_reflectable
		{
			template<typename U, U>
			struct Check;

			template<typename U>
			static std::true_type Test(Check<void(*)(TypeRegistry<T>&), &U::TypeOf>*);

			template<typename U>
			static std::false_type Test(...);

			static constexpr bool value = decltype(Test<T>(0))::value;
		};

		template<typename T>
		struct is_dynamically_reflectable
		{
			template<typename U, TypeInfo(U::*)() const>
			struct Check;

			template<typename U>
			static std::true_type Test(Check<U, &U::GetType>*);

			template<typename U>
			static std::false_type Test(...);

			static constexpr bool value = decltype(Test<T>(0))::value;
		};
	};

	struct TypeOfAccess
	{
		template <typename T>
		static auto TypeOf()
		{
			TypeRegistry<T> reg;
			Access::TypeOf(reg);
			return TypeInfo(reg.m_parameters);
		}
		template <typename C>
		static auto GetType(const C & object)
		{
			return Access::GetType(object);
		}
	};

	template <typename T>
	struct is_staticly_reflectable
	{
		static constexpr bool value = Access::is_staticly_reflectable<T>::value;
	};

	template <typename T>
	struct is_dynamically_reflectable
	{
		static constexpr bool value = Access::is_dynamically_reflectable<T>::value;
	};

	template <typename T, bool ref>
	struct TypeOfProxy
	{
		static auto TypeOf()
		{
			return TypeOfAccess::TypeOf<T>();
		}
		template <typename C>
		static auto GetType(const C & object)
		{
			return TypeOfAccess::GetType(object);
		}
	};
	template <typename T>
	struct TypeOfProxy<T, false>
	{
		static TypeInfo TypeOf()
		{
			TypeInfoParameters parameters;
			parameters.m_name = typeid(T).name();
			parameters.m_hash = typeid(T).hash_code();
			parameters.m_size = sizeof(T);
			return TypeInfo(parameters);
		}
	};
	template <>
	struct TypeOfProxy<void, false>
	{
		static TypeInfo TypeOf()
		{
			TypeInfoParameters parameters;
			parameters.m_name = typeid(void).name();
			parameters.m_hash = typeid(void).hash_code();
			return TypeInfo(parameters);
		}
	};

	struct TypeRegister
	{
		static TypeInfo Register(size_t hash, const std::function<TypeInfo()> & typeof);
	};

	template <typename T>
	static TypeInfo TypeOf()
	{
		typedef std::remove_pointer<std::decay<T>::type>::type raw_t;
		std::function<TypeInfo()> accessor = std::bind(&TypeOfProxy<raw_t, is_staticly_reflectable<raw_t>::value>::TypeOf);
		return TypeRegister::Register(typeid(raw_t).hash_code(), accessor);
	}

	template <typename T>
	static TypeInfo TypeOf(const T & object)
	{
		typedef std::remove_pointer<std::decay<T>::type>::type raw_t;
		static_assert(is_dynamically_reflectable<raw_t>::value, "Type is not dynamically reflectable");
		return TypeOfProxy<raw_t, is_dynamically_reflectable<raw_t>::value>::GetType(object);
	}
}

#endif //!COBALT_REFLECTION_HPP_INCLUDED
