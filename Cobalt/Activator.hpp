#ifndef COBALT_ACTIVATOR_HPP_INCLUDED
#define COBALT_ACTIVATOR_HPP_INCLUDED

namespace Cobalt
{
	struct Activator
	{
	public:
		template <typename T, typename ... ARGS>
		static auto CreateInstance(ARGS ... args)
		{
			auto type = TypeOf<T>();
			auto constructor = type.GetConstructor({ TypeOf<ARGS>()... });
			return constructor.Invoke<T>(std::forward<ARGS>(args)...);
		}

		template <typename ... ARGS>
		static auto CreateInstance(const TypeInfo & type, ARGS ... args)
		{
			auto constructor = type.GetConstructor({ TypeOf<ARGS>()... });
			auto result = constructor.Invoke({ Object(std::forward<ARGS>(args))... });
			if (type != result.GetType())
				throw std::exception("Can not create instance");
			return result;
		}

		template <typename ... ARGS>
		static auto CreateInstance(const TypeInfo && type, ARGS ... args)
		{
			auto constructor = type.GetConstructor({ TypeOf<ARGS>()... });
			auto result = constructor.Invoke({ Object(std::forward<ARGS>(args))... });
			if (type != result.GetType())
				throw std::exception("Can not create instance");
			return result;
		}
	};
}

#endif //!COBALT_ACTIVATOR_HPP_INCLUDED