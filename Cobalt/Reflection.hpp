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
		std::vector<FieldInfo> m_fields;
		std::vector<ConstructorInfo> m_constructors;
		std::vector<MethodInfo> m_methods;
		std::vector<PropertyInfo> m_properties;
	};

	template <typename T>
	struct TypeRegistry
	{
	public:
		TypeRegistry()
		{
			m_parameters.m_hash = typeid(T).hash_code();
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
		void Field(F (T::*field), const std::string && name)
		{
			auto accessor = [field](const Object && object)
			{
				auto obj = object.GetObject<T>();
				return Object(obj.*field);
			};
			m_parameters.m_fields.push_back(FieldInfo(name, TypeOf<F>(), accessor));
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
				return Object(T(std::forward<ARGS>(arguments[S].GetObject<ARGS>())...));
			}
		};

		template <typename ... ARGS>
		struct ParametersMaker
		{
			template<int ... S>
			static std::vector<ParameterInfo> Make(const std::vector<std::string> && names, sequence<S...>)
			{
				return{ ParameterInfo(names[S], TypeOf<ARGS>())... };
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
				return ConstructorMaker<ARGS...>::Make(std::move(arguments), typename gen_sequence<sizeof...(ARGS)>::type()).GetObject<T>();
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
				return Object((object.GetObject<T>().*method)(std::forward<ARGS>(arguments[S].GetObject<ARGS>())...));
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
				return Object((object.GetObject<T>().*method)(std::forward<ARGS>(arguments[S].GetObject<ARGS>())...));
			}
		};
		template <typename... ARGS>
		struct MethodMakerBase<void, ARGS...> : MethodMakerBaseEx<void, ARGS...>
		{
			template <typename M, int ... S>
			static Object MakeEx(const Object && object, M method, const std::vector<Object> && arguments, sequence<S...>)
			{
				arguments;
				(object.GetObject<T>().*method)(std::forward<ARGS>(arguments[S].GetObject<ARGS>())...);
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

		TypeInfoParameters m_parameters;
	};

	struct TypeOfAccess;
	template <typename T>
	struct is_reflectable;

	struct Access
	{
	private:
		friend struct TypeOfAccess;
		template <typename T>
		friend struct is_reflectable;

		template <typename T>
		static void TypeOf(TypeRegistry<T> & reg)
		{
			T::TypeOf(reg);
		}

		template <typename T>
		struct IsReflectable
		{
			template<typename U, U> struct Check;

			template<typename U>
			static std::true_type Test(Check<void(*)(TypeRegistry<T>&), &U::TypeOf>*);

			template<typename U>
			static std::false_type Test(...);

			static const bool value = decltype(Test<T>(0))::value;
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
	};

	template<typename T>
	struct is_reflectable
	{
		static constexpr bool value = Access::IsReflectable<T>::value;
	};

	template <typename T, bool ref>
	struct TypeOfProxy
	{
		static auto TypeOf()
		{
			return TypeOfAccess::TypeOf<T>();
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
			return TypeInfo(parameters);
		}
	};

	template <typename T>
	static TypeInfo TypeOf()
	{
		return TypeOfProxy<T, is_reflectable<T>::value>::TypeOf();
	}

	template <typename T>
	static TypeInfo TypeOf(T)
	{
		return TypeOfProxy<T, is_reflectable<T>::value>::TypeOf();
	}
}

#endif //!COBALT_REFLECTION_HPP_INCLUDED
