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

		template <typename R, typename... ARGS>
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
					return MethodMakerBase<R, ARGS...>::MakeEx(std::move(object), method, std::move(arguments), typename gen_sequence<sizeof...(ARGS)>::type());
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

		template <typename R, typename... ARGS>
		struct MethodMakerBase : MethodMakerBaseEx<R, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				return Object((object.As<T>().*method)(std::forward<ARGS>(arguments[S].As<ARGS>())...));
			}
		};
		template <typename... ARGS>
		struct MethodMakerBase<void, ARGS...> : MethodMakerBaseEx<void, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				(object.As<T>().*method)(std::forward<ARGS>(arguments[S].As<ARGS>())...);
				return Object();
			}
		};

		template <typename M>
		struct MethodMaker;
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...)> : MethodMakerBase<R, ARGS...>
		{
			static constexpr MethodModifier modifier = mmNone;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) const> : MethodMakerBase<R, ARGS...>
		{
			static constexpr MethodModifier modifier = mmConst;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) volatile> : MethodMakerBase<R, ARGS...>
		{
			static constexpr MethodModifier modifier = mmVolatile;
		};
		template <typename R, typename... ARGS>
		struct MethodMaker<R(T::*)(ARGS...) const volatile> : MethodMakerBase<R, ARGS...>
		{
			static constexpr MethodModifier modifier = mmConst | mmVolatile;
		};

		template <typename M>
		void Method(const std::string && name, M method, const std::vector<std::string> && names)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), MethodMaker<M>::MakeParameters(std::move(names)), MethodMaker<M>::modifier, MethodMaker<M>::Make(method)));
		}

		template <typename M>
		void Method(const std::string && name, M method)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), {}, MethodMaker<M>::modifier, MethodMaker<M>::Make(method)));
		}

		template <typename G, typename S>
		void Property(const std::string && name, G getter, S setter)
		{
			m_parameters.m_properties.push_back(PropertyInfo(
				std::move(name),
				MethodInfo(MethodMaker<G>::GetReturnType(), "get_" + name, {}, MethodMaker<G>::modifier, MethodMaker<G>::Make(getter)),
				MethodInfo(MethodMaker<S>::GetReturnType(), "set_" + name, MethodMaker<S>::MakeParameters({ std::move(name) }), MethodMaker<S>::modifier, MethodMaker<S>::Make(setter))
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
			return MethodInfo(MethodMaker<M>::GetReturnType(), std::move(name), MethodMaker<M>::MakeParameters(std::move(names)), MethodMaker<M>::modifier, MethodMaker<M>::Make(method));
		}

		static OperatorInfo MakeOperator(Cobalt::Operator oper, const std::vector<std::string> && names)
		{
			switch (oper)
			{
			case Cobalt::Operator::equality:
				return{ oper, MakeMethod("operator==", &T::operator==, std::move(names)) };
			}
			throw std::exception("Operator is not supported by Cobalt");
		};

		void Operator(Cobalt::Operator oper, const std::vector<std::string> && names)
		{
			m_parameters.m_operators.push_back(MakeOperator(oper, std::move(names)));
		}

		void Operator(Cobalt::Operator oper)
		{
			m_parameters.m_operators.push_back(MakeOperator(oper, {}));
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
		struct CallTypeOf
		{
			static auto TypeOf(const T & object)
			{
				return object.TypeOf();
			}
		};
		template <typename T>
		struct CallTypeOf<T, true>
		{
			static auto TypeOf(const T & object)
			{
				return object->TypeOf();
			}
		};

		template <typename T>
		static auto TypeOf(const T & object)
		{
			return CallTypeOf<T, std::is_pointer<T>::value>::TypeOf(object);
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
			static std::true_type Test(Check<U, &U::TypeOf>*);

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
		static auto TypeOf(const C & object)
		{
			return Access::TypeOf(object);
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
		static auto TypeOfStatic()
		{
			return TypeOfAccess::TypeOf<T>();
		}
		template <typename C>
		static auto TypeOfDynamic(const C & object)
		{
			return TypeOfAccess::TypeOf(object);
		}
	};
	template <typename T>
	struct TypeOfProxy<T, false>
	{
		static TypeInfo TypeOfStatic()
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
		static TypeInfo TypeOfStatic()
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
		std::function<TypeInfo()> accessor = std::bind(&TypeOfProxy<raw_t, is_staticly_reflectable<raw_t>::value>::TypeOfStatic);
		return TypeRegister::Register(typeid(raw_t).hash_code(), accessor);
	}

	template <typename T>
	static TypeInfo TypeOf(const T & object)
	{
		typedef std::remove_pointer<std::decay<T>::type>::type raw_t;
		static_assert(is_dynamically_reflectable<raw_t>::value, "Type is not dynamically reflectable");
		std::function<TypeInfo()> accessor = std::bind(&TypeOfProxy<raw_t, is_dynamically_reflectable<raw_t>::value>::TypeOfDynamic<T>, object);
		return TypeRegister::Register(typeid(raw_t).hash_code(), accessor);
	}
}

#endif //!COBALT_REFLECTION_HPP_INCLUDED
