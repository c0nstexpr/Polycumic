#pragma once

#include "property_base.h"
#include "getter/member_getter.h"
#include "getter/value_getter.h"
#include "setter/member_setter.h"
#include "setter/value_setter.h"

namespace polycumic::utility
{
    /**
     * \brief provide functionality like C# property
     */
    template<typename T, typename Getter, typename Setter, typename Validator = void>
    class property : public property_base<T, Getter, Setter, Validator>
    {
    public:
        using base = property_base<T, Getter, Setter, Validator>;
        using base::base;
        using typename base::type;
        using typename base::getter_result_t;
        using typename base::setter_result_t;
        using base::get_validator;

    private:
        using base::getter_;
        using base::setter_;
        using base::make_validated_setter;

    public:
        [[nodiscard]] constexpr decltype(auto) get() const { return getter_(); }

        [[nodiscard]] constexpr decltype(auto) operator()() const { return get(); }

        // ReSharper disable once CppNonExplicitConversionOperator
        [[nodiscard]] constexpr operator getter_result_t() const { return get(); }

        template<typename... Args> requires std::constructible_from<type, Args...>
        constexpr setter_result_t set(
            Args&&... args
        ) noexcept(
            std::is_nothrow_constructible_v<type, Args...> &&
            std::is_nothrow_invocable_r_v<validation_result, Validator, const type, const type>
        )
        {
            type new_value(std::forward<Args>(args)...);
            const validation_result result = get_validator()(get(), std::as_const(new_value));

            return result == validation_result::success ?
                setter_result_t{make_validated_setter(setter_, std::move(new_value))} :
                setter_result_t{tl::unexpect_t{}, result};
        }

        template<typename First = type, typename... Args>
        constexpr auto operator()(First&& first, Args&&... args)
        {
            auto&& res = set(std::forward<First>(first), std::forward<Args>(args)...);
            if(res) (*res)();
            else if(res.error() != validation_result::equaled)
                throw validation_error("");
        }

        template<typename U>
        constexpr property& operator=(U&& arg)
        {
            (*this)(std::forward<U>(arg));
            return *this;
        }
    };

    template<std::invocable Getter, typename Setter>
    requires getter_object<Getter, getter_value_t<Getter>> &&
    setter_object<Setter, getter_value_t<Getter>>
    constexpr auto make_property(
        Getter getter,
        Setter setter
    )
    noexcept(
        std::is_nothrow_constructible_v<
            property<getter_value_t<Getter>, Getter, Setter, void>,
            Getter&&,
            Setter&&>
    )
    {
        return property<getter_value_t<Getter>, Getter, Setter, void>(
            std::move(getter),
            std::move(setter)
        );
    }

    template<std::invocable Getter, typename Setter, typename Validator>
    requires getter_object<Getter, getter_value_t<Getter>> &&
    setter_object<Setter, getter_value_t<Getter>> &&
    validator<Validator, getter_value_t<Getter>>
    constexpr auto make_property(
        Getter getter,
        Setter setter,
        Validator validator
    )
    noexcept(
        std::is_nothrow_constructible_v<
            property<getter_value_t<Getter>, Getter, Setter, Validator>,
            Getter&&,
            Setter&&,
            Validator&&>
    )
    {
        return property<getter_value_t<Getter>, Getter, Setter, Validator>(
            std::move(getter),
            std::move(setter),
            std::move(validator)
        );
    }

    inline constexpr auto generate_value_property =
        [](auto& v) { return make_property(value_getter(v), value_setter(v)); };

    template<auto GetterMem, auto SetterMem>
    constexpr auto generate_property_from_mem(auto& instance)
    {
        using getter_traits = traits::member_function_pointer_traits<GetterMem>;
        using type = std::remove_cvref_t<typename getter_traits::result_t>;

        return make_property(
            member_getter<GetterMem>{instance},
            member_setter<type, SetterMem>(instance)
        );
    }
}
