#pragma once

#include <actors-framework/detail/callable_trait.hpp>
#include <actors-framework/detail/type_list.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::base {
    template<class List, std::size_t I>
    using forward_arg =
        typename std::conditional_t<std::is_lvalue_reference_v<type_traits::type_list_at_t<List, I>>,
                                    typename std::add_lvalue_reference_t<type_traits::decay_t<type_traits::type_list_at_t<List, I>>>,
                                    typename std::add_rvalue_reference_t<type_traits::decay_t<type_traits::type_list_at_t<List, I>>>>;
    /// type list to  Tuple
    template<class List>
    struct type_list_to_tuple;

    template<class... Ts>
    struct type_list_to_tuple<type_traits::type_list<Ts...>> {
        using type = std::tuple<type_traits::decay_t<Ts>...>;
    };

    template<class... Ts>
    using type_list_to_tuple_t = typename type_list_to_tuple<Ts...>::type;

    template<class F, std::size_t... I>
    void apply_impl(F&& f, communication_module& ctx, type_traits::index_sequence<I...>) {
        using call_trait = type_traits::get_callable_trait_t<type_traits::remove_reference_t<F>>;
        constexpr int args_size = call_trait::number_of_arguments;
        using args_type_list = type_traits::tl_slice_t<typename call_trait::args_types, 0, args_size>;
        using Tuple = type_list_to_tuple_t<args_type_list>;
        auto& args = ctx.current_message()->body<Tuple>();
        ///f(static_cast< forward_arg<args_type_list, I>>(std::get<I>(args))...);
        f((std::get<I>(args))...);
    }

    template<class F,
             class Args = typename type_traits::get_callable_trait<F>::args_types,
             int Args_size =
                 type_traits::get_callable_trait<F>::number_of_arguments>
    struct transformer {
        auto operator()(F&& f) -> std::function<void(communication_module&)> {
            return [f](communication_module& ctx) -> void {
                using call_trait =
                    type_traits::get_callable_trait_t<type_traits::remove_reference_t<F>>;
                constexpr int args_size = call_trait::number_of_arguments;
                apply_impl(f, ctx, type_traits::make_index_sequence<args_size>{});
            };
        }
    };

    /// class method
    template<class F, class ClassPtr, std::size_t... I>
    void apply_impl_for_class(F&& f, ClassPtr* ptr, communication_module& ctx, type_traits::index_sequence<I...>) {
        using call_trait = type_traits::get_callable_trait_t<type_traits::remove_reference_t<F>>;
        using args_type_list = typename call_trait::args_types;
        using Tuple = type_list_to_tuple_t<args_type_list>;
        auto& args = ctx.current_message()->body<Tuple>();
        //(ptr->*f)(static_cast< forward_arg<args_type_list, I>>(std::get<I>(args))...);
        (ptr->*f)((std::get<I>(args))...);
    }

    template<class F,
             class ClassPtr,
             class Args = typename type_traits::get_callable_trait<F>::args_types,
             int Args_size = type_traits::get_callable_trait<F>::number_of_arguments>
    struct transformer_for_class {
        auto operator()(F&& f, ClassPtr* ptr) -> std::function<void(communication_module&)> {
            return [f, ptr](communication_module& ctx) -> void {
                using call_trait = type_traits::get_callable_trait_t<type_traits::remove_reference_t<F>>;
                constexpr int args_size = call_trait::number_of_arguments;
                apply_impl_for_class(f, ptr, ctx, type_traits::make_index_sequence<args_size>{});
            };
        }
    };

    template<class F>
    helper::helper(F&& f) {
        helper_ = transformer<F>{}(std::forward<F>(f));
    }

    template<class F, class ClassPtr>
    helper::helper(F&& f, ClassPtr* self) {
        helper_ = transformer_for_class<F, ClassPtr>{}(std::forward<F>(f), self);
    }

} // namespace actors_framework::base
