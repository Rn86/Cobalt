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
			auto accessor = [field](const Value && value)
			{
				auto object = value.GetValue<T>();
				return Value(object.*field);
			};
			m_parameters.m_fields.push_back(FieldInfo(name, TypeOf<F>(), accessor));
		}

		template<int ...>
		struct seq { };

		template<int N, int ...S>
		struct gens : gens<N - 1, N - 1, S...> { };

		template<int ...S>
		struct gens<0, S...> {
			typedef seq<S...> type;
		};

		template <typename ... ARGS>
		struct ConstructorMaker
		{
			template<int ... S>
			static Value Make(const std::vector<Value> && arguments, seq<S...>)
			{
				return Value(T(std::forward<ARGS>(arguments[S].GetValue<ARGS>())...));
			}
		};

		template <typename ... ARGS>
		struct ParametersMaker
		{
			template<int ... S>
			static std::vector<ParameterInfo> Make(const std::vector<std::string> && names, seq<S...>)
			{
				return{ ParameterInfo(names[S], TypeOf<ARGS>())... };
			}
		};

		void Constructor()
		{
			auto accessor = [](const std::vector<Value> && values)
			{
				if (values.size() > 0)
					throw std::exception("Object can not be constructed");
				return Value(T());
			};
			m_parameters.m_constructors.push_back(ConstructorInfo({}, accessor));
		}

		template <typename ... ARGS>
		void Constructor(const std::vector<std::string> && names)
		{
			auto accessor = [](const std::vector<Value> && values)
			{
				return ConstructorMaker<ARGS...>::Make(std::move(values), typename gens<sizeof...(ARGS)>::type()).GetValue<T>();
			};
			std::vector<ParameterInfo> parameters = ParametersMaker<ARGS...>::Make(std::move(names), typename gens<sizeof...(ARGS)>::type());
			m_parameters.m_constructors.push_back(ConstructorInfo(parameters, accessor));
		}

		template <typename R, typename... ARGS>
		struct MethodMakerBase
		{
		public:
			template <typename M, int ... S>
			static Value Make(const Value && object, M method, const std::vector<Value> && arguments, seq<S...>)
			{
				arguments;
				return Value((object.GetValue<T>().*method)(std::forward<ARGS>(arguments[S].GetValue<ARGS>())...));
			}

			template <typename M>
			static std::function<Value(const Value &&, const std::vector<Value> &&)> Make(M method)
			{
				return[method](const Value && object, const std::vector<Value> && arguments)
				{
					return MethodMakerBase<R, ARGS...>::Make(std::move(object), method, std::move(arguments), typename gens<sizeof...(ARGS)>::type());
				};
			}

			static std::vector<ParameterInfo> MakeParameters(const std::string && names)
			{
				return ParametersMaker<ARGS...>::Make(names, typename gens<sizeof...(ARGS)>::type());
			}
			

			static TypeInfo GetReturnType()
			{
				return TypeOf<R>();
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
		void Method(const std::string && name, M method)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), MethodMaker<M>::MakeParameters(std::move(names)), MethodMaker<M>::modifier, MethodMaker<M>::Make(method)));
		}

		template <typename M>
		void Method(M method)
		{
			m_parameters.m_methods.push_back(MethodInfo(MethodMaker<M>::GetReturnType(), {}, MethodMaker<M>::modifier, MethodMaker<M>::Make(method)));
		}

		TypeInfoParameters m_parameters;
	};

	struct TypeOfAccess;

	struct Access
	{
	private:
		friend struct TypeOfAccess;

		template <typename T>
		static void TypeOf(TypeRegistry<T> & reg)
		{
			T::TypeOf(reg);
		}
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

	template <typename T>
	static auto TypeOf()
	{
		return TypeOfAccess::TypeOf<T>();
	}

	template <typename T>
	static auto TypeOf(T)
	{
		return TypeOfAccess::TypeOf<T>();
	}
}

#endif //!COBALT_REFLECTION_HPP_INCLUDED
